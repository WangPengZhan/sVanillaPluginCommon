#include "UrlProccess.h"

#include <regex>

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
}