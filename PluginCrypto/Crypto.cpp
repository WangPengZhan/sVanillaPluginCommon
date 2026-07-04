#include "Crypto.h"

#include "Encoding.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

namespace crypto
{
namespace
{
struct CipherCtxDeleter
{
    void operator()(EVP_CIPHER_CTX* ctx) const
    {
        EVP_CIPHER_CTX_free(ctx);
    }
};

struct MdCtxDeleter
{
    void operator()(EVP_MD_CTX* ctx) const
    {
        EVP_MD_CTX_free(ctx);
    }
};

struct PkeyCtxDeleter
{
    void operator()(EVP_PKEY_CTX* ctx) const
    {
        EVP_PKEY_CTX_free(ctx);
    }
};

struct PkeyDeleter
{
    void operator()(EVP_PKEY* pkey) const
    {
        EVP_PKEY_free(pkey);
    }
};

struct BioDeleter
{
    void operator()(BIO* bio) const
    {
        BIO_free(bio);
    }
};

using CipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, CipherCtxDeleter>;
using MdCtxPtr = std::unique_ptr<EVP_MD_CTX, MdCtxDeleter>;
using PkeyCtxPtr = std::unique_ptr<EVP_PKEY_CTX, PkeyCtxDeleter>;
using PkeyPtr = std::unique_ptr<EVP_PKEY, PkeyDeleter>;
using BioPtr = std::unique_ptr<BIO, BioDeleter>;

constexpr size_t aes128KeySize = 16;
constexpr size_t aesBlockSize = 16;
}  // namespace

std::string aes128Encrypt(const std::string& text, const std::string& mode, const std::string& key, const std::string& iv, const std::string& format)
{
    if (key.size() != aes128KeySize)
    {
        return {};
    }

    CipherCtxPtr ctx(EVP_CIPHER_CTX_new());
    if (!ctx)
    {
        return {};
    }

    const EVP_CIPHER* cipher = nullptr;

    if (mode == "cbc")
    {
        if (iv.size() != aesBlockSize)
        {
            return {};
        }
        cipher = EVP_aes_128_cbc();
    }
    else if (mode == "ecb")
    {
        cipher = EVP_aes_128_ecb();
    }
    else
    {
        return {};
    }

    const auto* keyData = reinterpret_cast<const unsigned char*>(key.c_str());
    const auto* ivData = reinterpret_cast<const unsigned char*>(iv.c_str());

    if (EVP_EncryptInit_ex2(ctx.get(), cipher, keyData, mode == "cbc" ? ivData : nullptr, nullptr) != 1)
    {
        return {};
    }
    if (EVP_CIPHER_CTX_set_padding(ctx.get(), 1) != 1)
    {
        return {};
    }

    const int maxLen = static_cast<int>(text.length()) + EVP_CIPHER_get_block_size(cipher);
    int cLen = 0;
    int fLen = 0;
    std::vector<unsigned char> ciphertext(maxLen);

    if (EVP_EncryptUpdate(ctx.get(), ciphertext.data(), &cLen, reinterpret_cast<const unsigned char*>(text.c_str()), static_cast<int>(text.length())) != 1)
    {
        return {};
    }
    if (EVP_EncryptFinal_ex(ctx.get(), ciphertext.data() + cLen, &fLen) != 1)
    {
        return {};
    }

    const std::string result(reinterpret_cast<char*>(ciphertext.data()), cLen + fLen);

    return format == "base64" ? encoding::base64Encode(result) : encoding::hexEncode(result);
}

std::string aes128EcbDecrypt(const std::string& ciphertext, const std::string& key, const std::string& iv, const std::string& format)
{
    if (key.size() != aes128KeySize)
    {
        return {};
    }

    const std::string decoded = format == "base64" ? encoding::base64Decode(ciphertext) : encoding::hexDecode(ciphertext);

    CipherCtxPtr ctx(EVP_CIPHER_CTX_new());
    if (!ctx)
    {
        return {};
    }

    const EVP_CIPHER* cipher = EVP_aes_128_ecb();

    const auto* keyData = reinterpret_cast<const unsigned char*>(key.c_str());

    if (EVP_DecryptInit_ex2(ctx.get(), cipher, keyData, nullptr, nullptr) != 1)
    {
        return {};
    }

    const int maxLen = static_cast<int>(decoded.length()) + EVP_CIPHER_get_block_size(cipher);
    int pLen = 0;
    int fLen = 0;
    std::vector<unsigned char> plaintext(maxLen);

    if (EVP_DecryptUpdate(ctx.get(), plaintext.data(), &pLen, reinterpret_cast<const unsigned char*>(decoded.c_str()), static_cast<int>(decoded.length())) != 1)
    {
        return {};
    }
    if (EVP_DecryptFinal_ex(ctx.get(), plaintext.data() + pLen, &fLen) != 1)
    {
        return {};
    }

    std::string result(reinterpret_cast<char*>(plaintext.data()), pLen + fLen);

    return result;
}

std::string rsaNoPaddingPublicEncryptHexLower(const std::string& text, const std::string& publicKey)
{
    BioPtr bio(BIO_new_mem_buf(publicKey.c_str(), -1));
    if (!bio)
    {
        return {};
    }

    PkeyPtr pkey(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr));
    if (!pkey)
    {
        return {};
    }

    PkeyCtxPtr ctx(EVP_PKEY_CTX_new(pkey.get(), nullptr));
    if (!ctx)
    {
        return {};
    }

    if (EVP_PKEY_encrypt_init(ctx.get()) <= 0)
    {
        return {};
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_NO_PADDING) <= 0)
    {
        return {};
    }

    const int keyLen = EVP_PKEY_get_size(pkey.get());
    if (keyLen <= 0)
    {
        return {};
    }

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
    if (EVP_PKEY_encrypt(ctx.get(), nullptr, &outLen, reinterpret_cast<const unsigned char*>(padded.data()), padded.length()) <= 0)
    {
        return {};
    }

    std::vector<unsigned char> encrypted(outLen);
    if (EVP_PKEY_encrypt(ctx.get(), encrypted.data(), &outLen, reinterpret_cast<const unsigned char*>(padded.data()), padded.length()) <= 0)
    {
        return {};
    }

    std::string encryptedStr = encoding::hexEncode(std::string(reinterpret_cast<char*>(encrypted.data()), outLen));
    std::transform(encryptedStr.begin(), encryptedStr.end(), encryptedStr.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return encryptedStr;
}

std::string md5Raw(const std::string& input)
{
    MdCtxPtr ctx(EVP_MD_CTX_new());
    if (!ctx)
    {
        return {};
    }

    if (EVP_DigestInit_ex(ctx.get(), EVP_md5(), nullptr) != 1)
    {
        return {};
    }

    if (EVP_DigestUpdate(ctx.get(), input.c_str(), input.length()) != 1)
    {
        return {};
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLen = 0;
    if (EVP_DigestFinal_ex(ctx.get(), digest, &digestLen) != 1)
    {
        return {};
    }

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
