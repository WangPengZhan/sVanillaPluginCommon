#pragma once
#include <string>

#include "CurlCpp/CurlResponseWrapper.h"
#include "CurlCpp/CurlWriter.h"

namespace network
{
struct LocationUrl
{
    std::string locationUrl;
    std::string body;
};

template <typename T>
class CurlResponseWrapper;

template <>
class CurlResponseWrapper<LocationUrl>
{
public:
    CurlResponseWrapper(LocationUrl& response)
        : m_response(response)
    {
    }

    void setToCurl(CURL* handle)
    {
        if (handle)
        {
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeFunc<std::string>);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &m_response.body);
        }
    }

    void setToCurl(CurlEasy& easy)
    {
        setToCurl(easy.handle());
    }

    void readAfter(CURL* handle)
    {
        char* redirectUrl = nullptr;
        curl_easy_getinfo(handle, CURLINFO_REDIRECT_URL, &redirectUrl);
        if (redirectUrl)
        {
            m_response.locationUrl = redirectUrl;
        }
    }

    void readAfter(CurlEasy& easy)
    {
        readAfter(easy.handle());
    }

private:
    LocationUrl& m_response;
};
}  // namespace network
