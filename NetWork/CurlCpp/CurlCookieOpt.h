
#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>

#include <curl/curl.h>

#include "CurlCookie.h"

namespace network
{
class CurlEasy;
class CurlCookieOpt : public CurlCookie
{
public:
    using CurlCookie::CurlCookie;
    CurlCookieOpt(const CurlCookie& cookie);
    std::string shortContent() const;

    void setToCurl(CURL* handle) const;
    void setToCurl(CurlEasy& easy) const;

private:
    std::string m_showContent;
};

}  // namespace network
