#pragma once
#include <string>

#include "CurlEasy.h"

namespace network
{

template <typename Context>
size_t writeFunc(void* data, size_t size, size_t nmemb, void* stream);

template <>
size_t writeFunc<std::string>(void* data, size_t size, size_t nmemb, void* stream);

template <>
size_t writeFunc<FILE*>(void* data, size_t size, size_t nmemb, void* stream);

template <typename Context>
class CurlWriter
{
public:
    CurlWriter(Context& ctx);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    bool isValid() const;

private:
    Context& m_context;
};

template <typename Context>
class CurlWriter<Context*>
{
public:
    CurlWriter(Context* ctx);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    bool isValid() const;

private:
    Context* m_context;
};

}  // namespace network

#include "CurlWriter.inl"
