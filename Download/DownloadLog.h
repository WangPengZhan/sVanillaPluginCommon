#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#define DOWNLOAD_LOG_TRACE(...)    SPDLOG_LOGGER_CALL(spdlog::get("Download"), spdlog::level::trace, __VA_ARGS__)
#define DOWNLOAD_LOG_DEBUG(...)    SPDLOG_LOGGER_CALL(spdlog::get("Download"), spdlog::level::debug, __VA_ARGS__)
#define DOWNLOAD_LOG_INFO(...)     SPDLOG_LOGGER_CALL(spdlog::get("Download"), spdlog::level::info, __VA_ARGS__)
#define DOWNLOAD_LOG_WARN(...)     SPDLOG_LOGGER_CALL(spdlog::get("Download"), spdlog::level::warn, __VA_ARGS__)
#define DOWNLOAD_LOG_ERROR(...)    SPDLOG_LOGGER_CALL(spdlog::get("Download"), spdlog::level::err, __VA_ARGS__)
#define DOWNLOAD_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get("Download"), spdlog::level::critical, __VA_ARGS__)
#define DOWNLOAD_LOG_OFF(...)      SPDLOG_LOGGER_CALL(spdlog::get("Download"), spdlog::level::off, __VA_ARGS__)
