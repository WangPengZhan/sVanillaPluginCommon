#include "UrlProccess.h"

#include <regex>
#include <string>
#include <unordered_set>
#include <string_view>

#include "PluginCrypto/Encoding.h"

namespace util
{

std::string u8ToString(const std::u8string& u8Str)
{
    return std::string(reinterpret_cast<const char*>(u8Str.data()), u8Str.size());
}

std::string urlEncode(const std::string& raw)
{
    return encoding::urlEncode(raw);
}

std::string urlDecode(const std::string& encoded)
{
    return encoding::urlDecode(encoded);
}

std::string removeSpecialChars(const std::string& input)
{
    std::unordered_set<char32_t> specialSet(std::begin(specialChars), std::end(specialChars));

    std::string result;
    std::u8string_view sv(reinterpret_cast<const char8_t*>(input.data()), input.size());

    for (size_t i = 0; i < sv.size();)
    {
        char32_t cp = 0;
        unsigned char lead = static_cast<unsigned char>(sv[i]);

        size_t length = 0;
        if (lead < 0x80)
        {
            cp = lead;
            length = 1;
        }
        else if ((lead >> 5) == 0x6)
        {
            length = 2;
        }
        else if ((lead >> 4) == 0xE)
        {
            length = 3;
        }
        else if ((lead >> 3) == 0x1E)
        {
            length = 4;
        }
        else
        {
            ++i;
            continue;
        }

        if (i + length > sv.size())
        {
            result.append(reinterpret_cast<const char*>(&sv[i]), sv.size() - i);
            break;
        }

        bool valid = true;
        for (size_t offset = 1; offset < length; ++offset)
        {
            unsigned char continuation = static_cast<unsigned char>(sv[i + offset]);
            if ((continuation >> 6) != 0x2)
            {
                valid = false;
                break;
            }
        }
        if (!valid)
        {
            result.append(reinterpret_cast<const char*>(&sv[i]), 1);
            ++i;
            continue;
        }

        if (length == 2)
        {
            cp = static_cast<char32_t>((lead & 0x1F) << 6 | (static_cast<unsigned char>(sv[i + 1]) & 0x3F));
        }
        else if (length == 3)
        {
            cp = static_cast<char32_t>((lead & 0x0F) << 12 | (static_cast<unsigned char>(sv[i + 1]) & 0x3F) << 6 |
                                      (static_cast<unsigned char>(sv[i + 2]) & 0x3F));
        }
        else if (length == 4)
        {
            cp = static_cast<char32_t>((lead & 0x07) << 18 | (static_cast<unsigned char>(sv[i + 1]) & 0x3F) << 12 |
                                      (static_cast<unsigned char>(sv[i + 2]) & 0x3F) << 6 | (static_cast<unsigned char>(sv[i + 3]) & 0x3F));
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
