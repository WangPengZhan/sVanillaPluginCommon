#include "CurlCookies.h"
#include "CurlCookie.h"
#include "CurlEasy.h"

namespace network
{

CurlCookies::CurlCookies(const std::string& context)
{
    setContent(context);
}

void CurlCookies::setContent(const std::string& context)
{
    *this = std::move(parseCookie(context));
}

std::string CurlCookies::cookieHeader(const std::string& domain) const
{
    std::string cookieString;
    cookieString.reserve(256);
    if (m_cookieValue.empty())
    {
        return "";
    }

    if (m_cookieValue.find(domain) == m_cookieValue.end())
    {
        return "";
    }

    for (const auto& valuePair : m_cookieValue.at(domain))
    {
        if (valuePair.second.empty())
        {
            continue;
        }

        if (valuePair.first == domain_key || valuePair.first == expires_key || valuePair.first == path_key || valuePair.first == same_site)
        {
            continue;
        }

        cookieString.append(valuePair.first);
        cookieString.append("=");
        cookieString.append(valuePair.second);
        cookieString.append("; ");
    }

    if (cookieString.size() > 2)
    {
        cookieString.resize(cookieString.size() - 2);
    }

    return cookieString;
}

CurlCookies::operator std::string() const
{
    std::string cookieString;
    for (const auto& pair : m_cookieValue)
    {
        cookieString += pair.first;
        cookieString += ": ";
        cookieString += cookie(pair.first);
        cookieString += "\r\n";
    }
    return cookieString;
}

CurlCookie CurlCookies::cookie(const std::string& domain) const
{
    CurlCookie curlCookie;
    if (contains(domain))
    {
        curlCookie.m_cookieValue = m_cookieValue.at(domain);
    }
    return curlCookie;
}

CurlCookies& CurlCookies::addCurlCookie(const CurlCookie& cookie)
{
    for (const auto& valuePair : cookie.m_cookieValue)
    {
        if (valuePair.first == cookie.domain())
        {
            continue;
        }

        m_cookieValue[cookie.domain()][valuePair.first] = valuePair.second;
    }

    return *this;
}

CurlCookies& CurlCookies::addCurlCookies(const CurlCookies& cookieJar)
{
    for (const auto& valuePair : cookieJar.m_cookieValue)
    {
        for (const auto& cookiePair : valuePair.second)
        {
            m_cookieValue[valuePair.first][cookiePair.first] = cookiePair.second;
        }
    }

    return *this;
}

bool CurlCookies::contains(const std::string& domain) const
{
    return m_cookieValue.find(domain) != m_cookieValue.end();
}

const std::string& CurlCookies::value(const std::string& key) const
{
    if (m_cookieValue.find(key) == m_cookieValue.end())
    {
        return m_empty;
    }
    else
    {
        static std::string content;
        content = cookie(key).content();
        return content;
    }
}

std::vector<std::string> CurlCookies::keys() const
{
    std::vector<std::string> ret;
    for (const auto& pair : m_cookieValue)
    {
        if (!pair.second.empty())
        {
            ret.push_back(pair.first);
        }
    }

    return ret;
}

CurlCookies CurlCookies::parseCookie(const std::string& content)
{
    CurlCookies cookies;
    cookies.addCurlCookie(CurlCookie::parseCookie(content));
    return cookies;
}

}  // namespace network
