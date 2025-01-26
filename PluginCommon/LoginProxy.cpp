#include "LoginProxy.h"
#include "BaseVideoView.h"

LoginProxy::LoginProxy(AbstractLoginApi& login)
    : AbstractLogin()
    , m_realLogin(login)
{
    setLoginWay(LoginWay::Api);
}

LoginProxy::LoginProxy(AbstractLoginWeb& login)
    : AbstractLogin()
    , m_realLogin(login)
{
    setLoginWay(LoginWay::Web);
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

AbstractLogin& LoginProxy::realLogin() const
{
    return m_realLogin;
}

int LoginProxy::type() const
{
    return m_realLogin.type();
}
