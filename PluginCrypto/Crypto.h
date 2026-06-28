#pragma once

#include <string>

namespace crypto
{
std::string base64Encode(const std::string& input);
std::string base64Decode(const std::string& input);

std::string hexEncode(const std::string& input);
std::string hexDecode(const std::string& input);

std::string aes128Encrypt(const std::string& text, const std::string& mode, const std::string& key, const std::string& iv,
                          const std::string& format = "base64");
std::string aes128EcbDecrypt(const std::string& ciphertext, const std::string& key, const std::string& iv, const std::string& format = "base64");

std::string rsaNoPaddingPublicEncryptHexLower(const std::string& text, const std::string& publicKey);

std::string md5Raw(const std::string& input);
std::string md5Hex(const std::string& input);
}  // namespace crypto
