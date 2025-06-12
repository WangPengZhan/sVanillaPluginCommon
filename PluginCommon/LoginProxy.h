#pragma once
#include <optional>
#include <set>

#include "ILogin.h"
#include "LoginApi.h"
#include "LoginWeb.h"

class LoginProxy : public AbstractLogin
{
public:
    LoginProxy(AbstractLoginApi& login);
    LoginProxy(AbstractLoginWeb& login);
    ~LoginProxy() = default;

    UserInfo getUserInfo(std::string dir) override;

    bool supportLogin() const override;
    bool isLogin() const override;
    std::string cookies() const override;
    bool refreshCookies(std::string cookies) override;
    void setCookies(std::string cookies) override;
    bool logout() override;

    std::vector<adapter::BaseVideoView> history() override;

    AbstractLogin& realLogin() const;

    int type() const override;

protected:
    AbstractLogin& m_realLogin;
};
