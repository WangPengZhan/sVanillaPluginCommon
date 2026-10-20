#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>

#include "CurlCookie.h"
#include "CurlCookieJar.h"
#include "CurlEasy.h"

namespace network
{
CurlCookieJar::CurlCookieJar(std::string filename)
    : m_filename(std::move(filename))
{
    if (!m_filename.empty() && !std::filesystem::exists(m_filename))
    {
        std::ofstream stream(m_filename);
    }
}

CurlCookieJar::CurlCookieJar(const CurlCookieJar& other)
{
    std::lock_guard lk(other.m_mutex);
    m_filename = other.m_filename;
}

CurlCookieJar& CurlCookieJar::operator=(const CurlCookieJar& other)
{
    if (this == &other)
    {
        return *this;
    }

    std::scoped_lock lk(m_mutex, other.m_mutex);
    m_filename = other.m_filename;
    m_cacheValid = false;
    return *this;
}

CurlCookieJar::operator bool() const
{
    if (m_filename.empty())
    {
        return false;
    }

    return std::filesystem::exists(m_filename);
}

bool CurlCookieJar::contains(const std::string& key) const
{
    return toCookies().contains(key);
}

std::string CurlCookieJar::value(const std::string& key) const
{
    return toCookies().value(key);
}

std::vector<std::string> CurlCookieJar::keys() const
{
    return toCookies().keys();
}

void CurlCookieJar::setToCurl(CURL* handle)
{
    if (handle && this->operator bool())
    {
        curl_easy_setopt(handle, CURLOPT_COOKIEJAR, m_filename.c_str());
        curl_easy_setopt(handle, CURLOPT_COOKIEFILE, m_filename.c_str());
    }
}

void CurlCookieJar::setToCurl(CurlEasy& easy)
{
    setToCurl(easy.handle());
}

CurlCookies CurlCookieJar::toCookies() const
{
    if (!this->operator bool())
    {
        std::lock_guard lk(m_mutex);
        m_cacheValid = false;
        return CurlCookies();
    }

    std::error_code errorCode;
    const auto writeTime = std::filesystem::last_write_time(m_filename, errorCode);
    {
        std::lock_guard lk(m_mutex);
        if (m_cacheValid && !errorCode && m_cacheTime == writeTime)
        {
            return m_cache;
        }
    }

    CurlCookies cookies;
    std::ifstream inputFile(m_filename);
    if (!inputFile)
    {
        return cookies;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    cookies = CurlCookies::parseCookies(buffer.str());

    if (!errorCode)
    {
        std::lock_guard lk(m_mutex);
        m_cache = cookies;
        m_cacheTime = writeTime;
        m_cacheValid = true;
    }

    return cookies;
}

}  // namespace network
