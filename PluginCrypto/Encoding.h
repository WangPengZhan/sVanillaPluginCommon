#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace encoding
{
std::string base64Encode(const std::string& input);
std::string base64Decode(const std::string& input);

std::string hexEncode(const std::string& input);
std::string hexDecode(const std::string& input);

std::string urlEncode(const std::string& decoded);
std::string urlDecode(const std::string& encoded);

std::vector<uint32_t> utf8ToCodePoints(const std::string& str);
std::string codePointsToUtf8(const std::vector<uint32_t>& cps);
}  // namespace encoding
