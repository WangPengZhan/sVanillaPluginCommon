#pragma once

namespace network
{

class CurlGlobal
{
public:
    static CurlGlobal& instance();

private:
    CurlGlobal();
    CurlGlobal(long flag);
    ~CurlGlobal();

    CurlGlobal(const CurlGlobal&) = delete;
    CurlGlobal& operator=(const CurlGlobal&) = delete;
    CurlGlobal(CurlGlobal&&) = delete;
    CurlGlobal& operator=(CurlGlobal&&) = delete;
};

}  // namespace network
