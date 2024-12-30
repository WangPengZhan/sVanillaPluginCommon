#pragma once
#include "Login.h"

class LoginProxy : public AbstractLogin
{
public:
    LoginProxy(AbstractLogin& login);
    ~LoginProxy() = default;

    LoginSatus getLoginStatus() override;
    bool getScanContext(std::string& content) override;
    void loginSuccess() override;
    UserInfo getUserInfo(std::string dir) override;
    bool isLogin() const override;
    bool logout() override;
    std::vector<adapter::BaseVideoView> history() override;
    const LoginResource& allResources() const override;
    const std::vector<uint8_t>& resource(ResourceIndex index) const override;
    int type() const override;

protected:
    AbstractLogin& m_realLogin;
};
