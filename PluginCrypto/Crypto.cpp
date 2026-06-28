#include "Crypto.h"

#include "Encoding.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <vector>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

namespace crypto
{
std::string aes128Encrypt(const std::string& text, const std::string& mode, const std::string& key, const std::string& iv, const std::string& format)
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = nullptr;

    if (mode == "cbc")
    {
        cipher = EVP_aes_128_cbc();
    }
    else if (mode == "ecb")
    {
        cipher = EVP_aes_128_ecb();
    }
    else
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    const auto* keyData = reinterpret_cast<const unsigned char*>(key.c_str());
    const auto* ivData = reinterpret_cast<const unsigned char*>(iv.c_str());

    EVP_EncryptInit_ex(ctx, cipher, nullptr, keyData, iv.empty() ? nullptr : ivData);
    EVP_CIPHER_CTX_set_padding(ctx, 1);

    const int maxLen = static_cast<int>(text.length()) + EVP_CIPHER_block_size(cipher);
    int cLen = 0;
    int fLen = 0;
    std::vector<unsigned char> ciphertext(maxLen);

    EVP_EncryptUpdate(ctx, ciphertext.data(), &cLen, reinterpret_cast<const unsigned char*>(text.c_str()), static_cast<int>(text.length()));
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + cLen, &fLen);

    const std::string result(reinterpret_cast<char*>(ciphertext.data()), cLen + fLen);
    EVP_CIPHER_CTX_free(ctx);

    return format == "base64" ? encoding::base64Encode(result) : encoding::hexEncode(result);
}

std::string aes128EcbDecrypt(const std::string& ciphertext, const std::string& key, const std::string& iv, const std::string& format)
{
    const std::string decoded = format == "base64" ? encoding::base64Decode(ciphertext) : encoding::hexDecode(ciphertext);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = EVP_aes_128_ecb();

    auto* keyData = reinterpret_cast<unsigned char*>(const_cast<char*>(key.c_str()));
    auto* ivData = reinterpret_cast<unsigned char*>(const_cast<char*>(iv.c_str()));

    EVP_DecryptInit_ex(ctx, cipher, nullptr, keyData, iv.empty() ? nullptr : ivData);

    int pLen = static_cast<int>(decoded.length());
    int fLen = 0;
    std::vector<unsigned char> plaintext(pLen + AES_BLOCK_SIZE);

    EVP_DecryptUpdate(ctx, plaintext.data(), &pLen, reinterpret_cast<const unsigned char*>(decoded.c_str()), static_cast<int>(decoded.length()));
    EVP_DecryptFinal_ex(ctx, plaintext.data() + pLen, &fLen);

    std::string result(reinterpret_cast<char*>(plaintext.data()), pLen + fLen);
    EVP_CIPHER_CTX_free(ctx);

    return result;
}

std::string rsaNoPaddingPublicEncryptHexLower(const std::string& text, const std::string& publicKey)
{
    BIO* bio = BIO_new_mem_buf(publicKey.c_str(), -1);
    if (!bio)
    {
        return {};
    }

    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    if (!pkey)
    {
        BIO_free(bio);
        return {};
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx)
    {
        EVP_PKEY_free(pkey);
        BIO_free(bio);
        return {};
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        BIO_free(bio);
        return {};
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_NO_PADDING) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        BIO_free(bio);
        return {};
    }

    RSA* rsa = EVP_PKEY_get1_RSA(pkey);
    const int keyLen = RSA_size(rsa);
    RSA_free(rsa);

    std::string padded = text;
    if (padded.size() < static_cast<size_t>(keyLen))
    {
        padded.insert(padded.begin(), keyLen - padded.size(), '\0');
    }
    else if (padded.size() > static_cast<size_t>(keyLen))
    {
        padded = padded.substr(0, keyLen);
    }

    size_t outLen = 0;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outLen, reinterpret_cast<const unsigned char*>(padded.c_str()), padded.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        BIO_free(bio);
        return {};
    }

    std::vector<unsigned char> encrypted(outLen);
    if (EVP_PKEY_encrypt(ctx, encrypted.data(), &outLen, reinterpret_cast<const unsigned char*>(padded.c_str()), padded.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        BIO_free(bio);
        return {};
    }

    std::string encryptedStr = encoding::hexEncode(std::string(reinterpret_cast<char*>(encrypted.data()), outLen));
    std::transform(encryptedStr.begin(), encryptedStr.end(), encryptedStr.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    BIO_free(bio);

    return encryptedStr;
}

std::string md5Raw(const std::string& input)
{
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        return {};
    }

    if (EVP_DigestInit_ex(ctx, EVP_md5(), nullptr) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }

    if (EVP_DigestUpdate(ctx, input.c_str(), input.length()) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLen = 0;
    if (EVP_DigestFinal_ex(ctx, digest, &digestLen) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return {};
    }

    EVP_MD_CTX_free(ctx);

    return std::string(reinterpret_cast<char*>(digest), digestLen);
}

std::string md5Hex(const std::string& input)
{
    std::stringstream ss;
    const std::string digest = md5Raw(input);
    for (const unsigned char c : digest)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return ss.str();
}
}  // namespace crypto
