#pragma once
#include <array>
#include <string>
#include <vector>

#include "ILogin.h"

class AbstractLoginApi : public AbstractLogin
{
public:
    enum LoginStatus
    {
        Unknow,
        Error,
        NoScan,
        Timeout,
        ScannedNoAck,
        Success
    };

    enum ResourceIndex
    {
        Background,
        Loading,
        Tip,
        WaitConfirm,
        Complete,
        Confirmed,
        Refresh
    };

    using LoginResource = std::array<std::vector<uint8_t>, 7>;

    // thread-safe
    virtual LoginStatus getLoginStatus() = 0;
    virtual bool getScanContext(std::string& content) = 0;
    virtual void loginSuccess() = 0;

    // resource
    virtual const LoginResource& allResources() const = 0;
    virtual const std::vector<uint8_t>& resource(ResourceIndex index) const = 0;

protected:
    std::vector<uint8_t> m_emptyString;
    std::array<std::vector<uint8_t>, 7> m_emptyArray;
};
