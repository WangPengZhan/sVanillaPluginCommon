#include "LocaleHelper.h"

#ifdef _WIN32
#    include <windows.h>
#endif

namespace util
{

std::string localeToUtf8(const std::string& localeStr)
{
#ifdef _WIN32
    int len = MultiByteToWideChar(GetACP(), 0, localeStr.data(), localeStr.size(), nullptr, 0);
    std::wstring wsz_utf8(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, localeStr.data(), localeStr.size(), &wsz_utf8[0], len);

    len = WideCharToMultiByte(CP_UTF8, 0, wsz_utf8.data(), wsz_utf8.size(), nullptr, 0, nullptr, nullptr);
    std::string strTemp(len, '\0');
    WideCharToMultiByte(GetACP(), 0, wsz_utf8.data(), wsz_utf8.size(), &strTemp[0], len, nullptr, nullptr);

#else
    std::string strTemp = localeStr;
#endif

    return strTemp;
}

std::string utf8ToLocale(const std::string& utf8Str)
{
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), utf8Str.size(), nullptr, 0);
    std::wstring wsz_ansi(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), utf8Str.size(), &wsz_ansi[0], len);

    len = WideCharToMultiByte(GetACP(), 0, wsz_ansi.data(), wsz_ansi.size(), nullptr, 0, nullptr, nullptr);
    std::string strTemp(len, '\0');
    WideCharToMultiByte(GetACP(), 0, wsz_ansi.data(), wsz_ansi.size(), &strTemp[0], len, nullptr, nullptr);

#else
    std::string strTemp = utf8Str;
#endif

    return strTemp;
}

}  // namespace util
