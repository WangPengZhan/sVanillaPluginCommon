
#pragma once
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "CurlCookies.h"

namespace network
{

class CurlEasy;

class CurlCookieJar
{
public:
    using CurlCookieJarValueType = std::map<std::string, std::string>;

    CurlCookieJar() = default;
    CurlCookieJar(std::string filename);
    CurlCookieJar(const CurlCookieJar& other);
    CurlCookieJar& operator=(const CurlCookieJar& other);

    operator bool() const;

    bool contains(const std::string& key) const;
    std::string value(const std::string& key) const;
    std::vector<std::string> keys() const;

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    CurlCookies toCookies() const;

private:
    std::string m_filename;

    // curl rewrites the jar file after each request, so the parsed copy is only
    // reused while the last write time is unchanged. Without it every
    // contains()/value()/keys() call re-read and re-parsed the whole file.
    mutable std::mutex m_mutex;
    mutable bool m_cacheValid = false;
    mutable std::filesystem::file_time_type m_cacheTime{};
    mutable CurlCookies m_cache;
};

}  // namespace network
