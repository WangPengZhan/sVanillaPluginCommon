#include "ILogin.h"

AbstractLogin::LoginWay AbstractLogin::loginWay() const
{
    return m_loginWay;
}

void AbstractLogin::setLoginWay(LoginWay way)
{
    m_loginWay = way;
}
