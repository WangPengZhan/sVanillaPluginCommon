#pragma once

namespace network
{

template <typename CurlOption>
inline void CurlEasy::setOption(typename CurlOption::ValueType const& value)
{
    CurlOption option(value);
    setOption(option);
}

template <typename InputIterator>
inline void CurlEasy::setOption(InputIterator first, InputIterator last)
{
    for (; first != last; ++first)
    {
        setOption(*first);
    }
}

template <typename T>
inline void CurlEasy::getInfo(CURLINFO info, T& value) const
{
    const CURLcode code = curl_easy_getinfo(handle(), info, &value);
}
}  // namespace network
