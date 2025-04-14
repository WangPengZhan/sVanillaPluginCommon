#pragma once
#include <string>

struct LoggerRegisterHelpper
{
    static void registerLogger(std::string name, std::string path);
    static void unregisterLogger(std::string name);
};
