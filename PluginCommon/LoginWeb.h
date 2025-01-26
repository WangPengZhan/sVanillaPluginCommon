#pragma once
#include <array>
#include <string>
#include <vector>
#include <unordered_set>

#include "ILogin.h"

class AbstractLoginWeb : public AbstractLogin
{
public:
    AbstractLoginWeb()
        : AbstractLogin()
    {
        setLoginWay(LoginWay::Web);
    }

    virtual std::string loginUrl() const = 0;
    virtual void setCookies(std::string cookies) const = 0;
    virtual std::unordered_set<std::string> mustKeys() const = 0;
};
