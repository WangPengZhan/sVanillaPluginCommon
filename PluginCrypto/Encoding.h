#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace encoding
{
std::string urlEncode(const std::string& decoded);
std::string urlDecode(const std::string& encoded);

std::vector<uint32_t> utf8ToCodePoints(const std::string& str);
std::string codePointsToUtf8(const std::vector<uint32_t>& cps);
}  // namespace encoding
