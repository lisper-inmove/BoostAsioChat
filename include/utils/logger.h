#pragma once

#include <server/config.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace chat::utils::logger {
bool initLogger(const server::config::LogConfig &config);

#define LOG_TRACE(...)                                                         \
  spdlog::log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},            \
              spdlog::level::info, __VA_ARGS__)
#define LOG_DEBUG(...)                                                         \
  spdlog::log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},            \
              spdlog::level::info, __VA_ARGS__)
#define LOG_INFO(...)                                                          \
  spdlog::log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},            \
              spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...)                                                          \
  spdlog::log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},            \
              spdlog::level::info, __VA_ARGS__)
#define LOG_CRITICAL(...)                                                      \
  spdlog::log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},            \
              spdlog::level::info, __VA_ARGS__)
#define LOG_ERR(...)                                                           \
  spdlog::log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__},            \
              spdlog::level::info, __VA_ARGS__)
} // namespace chat::utils::logger
