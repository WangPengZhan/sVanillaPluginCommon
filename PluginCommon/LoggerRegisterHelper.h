#pragma once
#include <string>

struct LoggerHelper
{
    static void registerLogger(std::string name, std::string path);
    static void unregisterLogger(std::string name);
};
