#include "CurlCookieOpt.h"
#include "CurlEasy.h"

namespace network
{
CurlCookieOpt::CurlCookieOpt(const CurlCookie& cookie)
    : CurlCookie(cookie)
{
}

std::string CurlCookieOpt::shortContent() const
{
    std::string cookieString;
    for (const auto& valuePair : m_cookieValue)
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

void CurlCookieOpt::setToCurl(CURL* handle) const
{
    m_showContent = shortContent();
    if (handle && !m_showContent.empty())
    {
        curl_easy_setopt(handle, CURLOPT_COOKIE, m_showContent.c_str());
    }
}

void CurlCookieOpt::setToCurl(CurlEasy& easy) const
{
    setToCurl(easy.handle());
}

}  // namespace network
