#include "CurlCookies.h"
#include "CurlCookie.h"
#include "CurlEasy.h"

namespace network
{

CurlCookies::CurlCookies(const std::string& context)
{
    *this = parseCookies(context);
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

CurlCookies CurlCookies::parseCookies(const std::string& content)
{
    CurlCookies cookies;

    std::string_view str(content);
    while (!str.empty())
    {
        size_t lineEnd = str.find("\r\n");
        std::string_view line = (lineEnd == std::string_view::npos) ? str : str.substr(0, lineEnd);

        size_t sep = line.find(": ");
        if (sep != std::string_view::npos)
        {
            std::string_view key = line.substr(0, sep);
            std::string_view value = line.substr(sep + 2);
            cookies.addCurlCookie(CurlCookie::parseCookie(std::string(value)));
        }

        if (lineEnd == std::string_view::npos)
        {
            break;
        }
        str.remove_prefix(lineEnd + 2);
    }

    return cookies;
}

}  // namespace network
