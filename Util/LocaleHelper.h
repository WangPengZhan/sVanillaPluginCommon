#pragma once
#include <string>

namespace util
{

std::string localeToUtf8(const std::string& localeStr);
std::string utf8ToLocale(const std::string& utf8Str);

}  // namespace util
