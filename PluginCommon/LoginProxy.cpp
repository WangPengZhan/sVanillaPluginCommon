#include "LoginProxy.h"
#include "BaseVideoView.h"

LoginProxy::LoginProxy(AbstractLogin& login)
    : AbstractLogin()
    , m_realLogin(login)
{
}

LoginProxy::LoginSatus LoginProxy::getLoginStatus()
{
    return m_realLogin.getLoginStatus();
}

bool LoginProxy::getScanContext(std::string& content)
{
    return m_realLogin.getScanContext(content);
}

void LoginProxy::loginSuccess()
{
    m_realLogin.loginSuccess();
}

UserInfo LoginProxy::getUserInfo(std::string dir)
{
    return m_realLogin.getUserInfo(dir);
}

bool LoginProxy::isLogin() const
{
    return m_realLogin.isLogin();
}

bool LoginProxy::logout()
{
    return m_realLogin.logout();
}

std::vector<adapter::BaseVideoView> LoginProxy::history()
{
    return m_realLogin.history();
}

const LoginProxy::LoginResource& LoginProxy::allResources() const
{
    return m_realLogin.allResources();
}

const std::vector<uint8_t>& LoginProxy::resource(ResourceIndex index) const
{
    return m_realLogin.resource(index);
}

int LoginProxy::type() const
{
    return m_realLogin.type();
}
