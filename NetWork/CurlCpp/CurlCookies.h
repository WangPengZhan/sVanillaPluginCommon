
#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include <curl/curl.h>

namespace network
{
class CurlCookie;

class CurlCookies
{
public:
    using CurlCookiesValueType = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;

    CurlCookies() = default;
    CurlCookies(const std::string& context);

    std::string cookieHeader(const std::string& domain) const;
    operator std::string() const;

    CurlCookie cookie(const std::string& domain) const;

    CurlCookies& addCurlCookie(const CurlCookie& cookie);
    CurlCookies& addCurlCookies(const CurlCookies& cookieJar);

    bool contains(const std::string& domain) const;
    const std::string& value(const std::string& key) const;
    std::vector<std::string> keys() const;

    static CurlCookies parseCookies(const std::string& content);

private:
    CurlCookiesValueType m_cookieValue;
    std::string m_empty;
};

}  // namespace network
