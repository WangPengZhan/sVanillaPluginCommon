#pragma once
#include <array>
#include <string>
#include <vector>

struct UserInfo
{
    std::string facePath;
    std::string uname;
    std::string vipType;
};

namespace adapter
{
struct BaseVideoView;
}  // namespace adapter

class AbstractLogin
{
public:
    enum LoginWay
    {
        Unknow,
        Web,
        Api,
        Account,
    };

    virtual ~AbstractLogin() = default;

    virtual bool isLogin() const = 0;
    virtual std::string cookies() const = 0;
    virtual void setCookies(std::string cookies) = 0;
    virtual bool refreshCookies(std::string cookies) = 0;
    virtual bool logout() = 0;

    virtual UserInfo getUserInfo(std::string dir) = 0;
    virtual std::vector<adapter::BaseVideoView> history() = 0;

    virtual int type() const = 0;

    virtual LoginWay loginWay() const;

protected:
    void setLoginWay(LoginWay way);

protected:
    LoginWay m_loginWay;
};
