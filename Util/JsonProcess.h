#pragma once
#include <nlohmann/json_fwd.hpp>

namespace util
{

class JsonProcess
{
public:
    static void removeNullValues(nlohmann::json& jsonObj);
};

}  // namespace util
