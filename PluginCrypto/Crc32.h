#pragma once

#include <cstdint>
#include <string>

namespace checksum
{
uint32_t crc32(const std::string& input);
}  // namespace checksum
