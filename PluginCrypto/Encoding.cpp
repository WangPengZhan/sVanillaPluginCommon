#include "Encoding.h"

#include <cctype>
#include <iomanip>
#include <sstream>

namespace encoding
{
namespace
{
bool isUnreserved(unsigned char c)
{
    return std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~';
}

int hexValue(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    return -1;
}
}  // namespace

std::string urlEncode(const std::string& decoded)
{
    std::ostringstream output;
    output << std::uppercase << std::hex;

    for (const unsigned char c : decoded)
    {
        if (isUnreserved(c))
        {
            output << static_cast<char>(c);
        }
        else
        {
            output << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
    }

    return output.str();
}

std::string urlDecode(const std::string& encoded)
{
    std::string result;
    result.reserve(encoded.size());

    for (size_t i = 0; i < encoded.size(); ++i)
    {
        if (encoded[i] == '%' && i + 2 < encoded.size())
        {
            const int high = hexValue(encoded[i + 1]);
            const int low = hexValue(encoded[i + 2]);
            if (high >= 0 && low >= 0)
            {
                result.push_back(static_cast<char>((high << 4) | low));
                i += 2;
                continue;
            }
        }

        result.push_back(encoded[i]);
    }

    return result;
}

std::vector<uint32_t> utf8ToCodePoints(const std::string& str)
{
    std::vector<uint32_t> cps;
    const auto* p = reinterpret_cast<const uint8_t*>(str.data());
    const size_t len = str.size();
    size_t i = 0;
    while (i < len)
    {
        uint32_t c = p[i];
        if (c < 0x80)
        {
            cps.push_back(c);
            i++;
        }
        else if (c < 0xE0 && i + 1 < len && (p[i + 1] >> 6) == 0x2)
        {
            cps.push_back(((c & 0x1F) << 6) | (p[i + 1] & 0x3F));
            i += 2;
        }
        else if (c < 0xF0 && i + 2 < len && (p[i + 1] >> 6) == 0x2 && (p[i + 2] >> 6) == 0x2)
        {
            cps.push_back(((c & 0x0F) << 12) | ((p[i + 1] & 0x3F) << 6) | (p[i + 2] & 0x3F));
            i += 3;
        }
        else if (c < 0xF8 && i + 3 < len && (p[i + 1] >> 6) == 0x2 && (p[i + 2] >> 6) == 0x2 && (p[i + 3] >> 6) == 0x2)
        {
            cps.push_back(((c & 0x07) << 18) | ((p[i + 1] & 0x3F) << 12) | ((p[i + 2] & 0x3F) << 6) | (p[i + 3] & 0x3F));
            i += 4;
        }
        else
        {
            cps.push_back(c);
            i++;
        }
    }
    return cps;
}

std::string codePointsToUtf8(const std::vector<uint32_t>& cps)
{
    std::string out;
    for (uint32_t c : cps)
    {
        if (c < 0x80)
        {
            out += static_cast<char>(c);
        }
        else if (c < 0x800)
        {
            out += static_cast<char>(0xC0 | (c >> 6));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
        else if (c < 0x10000)
        {
            out += static_cast<char>(0xE0 | (c >> 12));
            out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
        else
        {
            out += static_cast<char>(0xF0 | (c >> 18));
            out += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
    }
    return out;
}
}  // namespace encoding
