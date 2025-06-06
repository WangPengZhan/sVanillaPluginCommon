#include "UrlProccess.h"

#include <regex>
#include <string>
#include <unordered_set>
#include <string_view>

namespace util
{
bool isUri(const std::string& uri)
{
    static constexpr auto patternStr = "^(?:(?:https?|ftp):\\/\\/)?"
                                       "(?:\\S+(?::\\S*)?@)?(?:(?!(?:10|127)(?:\\.\\d{1,3}){3})"
                                       "(?!(?:169\\.254|192\\.168)(?:\\.\\d{1,3}){2})(?!172\\.(?:1[6-9]|2\\d|3[0-1])"
                                       "(?:\\.\\d{1,3}){2})(?:[1-9]\\d?|1\\d\\d|2[01]\\d|22[0-3])"
                                       "(?:\\.(?:1?\\d{1,2}|2[0-4]\\d|25[0-5])){2}"
                                       "(?:\\.(?:[1-9]\\d?|1\\d\\d|2[0-4]\\d|25[0-4]))|(?:(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)"
                                       "(?:\\.(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)*(?:\\.(?:[a-z\u00a1-\uffff]{2,}))\\.?)"
                                       "(?::\\d{2,5})?(?:[/?#]\\S*)?$";

    std::regex pattern(patternStr);
    return std::regex_search(uri, pattern);
}

std::string u8ToString(const std::u8string& u8Str)
{
    return std::string(reinterpret_cast<const char*>(u8Str.data()), u8Str.size());
}

std::string urlDecode(const std::string& encoded)
{
    std::string decoded;
    for (size_t i = 0; i < encoded.length(); ++i)
    {
        if (encoded[i] == '%')
        {
            if (i + 2 < encoded.length())
            {
                std::string hex = encoded.substr(i + 1, 2);
                char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
                decoded += decodedChar;
                i += 2;
            }
        }
        else
        {
            decoded += encoded[i];
        }
    }
    return decoded;
}

std::string removeSpecialChars(const std::string& input)
{
    std::unordered_set<char32_t> specialSet(std::begin(specialChars), std::end(specialChars));

    std::string result;
    std::u8string_view sv(reinterpret_cast<const char8_t*>(input.data()), input.size());

    for (size_t i = 0; i < sv.size();)
    {
        char32_t cp = 0;
        unsigned char lead = sv[i];

        size_t length = 0;
        if (lead < 0x80)
        {
            cp = lead;
            length = 1;
        }
        else if ((lead >> 5) == 0x6)
        {
            cp = (lead & 0x1F) << 6 | (sv[i + 1] & 0x3F);
            length = 2;
        }
        else if ((lead >> 4) == 0xE)
        {
            cp = (lead & 0x0F) << 12 | (sv[i + 1] & 0x3F) << 6 | (sv[i + 2] & 0x3F);
            length = 3;
        }
        else if ((lead >> 3) == 0x1E)
        {
            cp = (lead & 0x07) << 18 | (sv[i + 1] & 0x3F) << 12 | (sv[i + 2] & 0x3F) << 6 | (sv[i + 3] & 0x3F);
            length = 4;
        }
        else
        {
            ++i;
            continue;
        }

        if (specialSet.find(cp) == specialSet.end())
        {
            result.append(reinterpret_cast<const char*>(&sv[i]), length);
        }

        i += length;
    }

    return result;
}

}  // namespace util
