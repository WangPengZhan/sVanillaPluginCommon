#include "LoggerRegisterHelper.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace
{
constexpr int logFileMaxSize = 100 * 1024 * 1024;
constexpr int logFileMaxCount = 100;

template <typename T>
void freeObject(T* obj)
{
    static_assert(!std::is_void_v<T>, "Cannot delete void pointer");
    if (obj)
    {
        delete obj;
    }
}
}  // namespace

void LoggerHelper::registerLogger(std::string name, std::string path)
{
    using rotating_file_sink_mt = spdlog::sinks::rotating_file_sink_mt;
    auto sink =
        std::shared_ptr<rotating_file_sink_mt>(new rotating_file_sink_mt(std::move(path), logFileMaxSize, logFileMaxCount), freeObject<rotating_file_sink_mt>);
    auto new_logger = std::shared_ptr<spdlog::logger>(new spdlog::logger(name, std::move(sink)), freeObject<spdlog::logger>);
    spdlog::details::registry::instance().initialize_logger(new_logger);
}

void LoggerHelper::unregisterLogger(std::string name)
{
    spdlog::details::registry::instance().drop(name);
}
