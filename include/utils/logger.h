#pragma once

#include <server/config.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace chat::utils::logger {
bool initLogger(const server::config::LogConfig &config);
} // namespace chat::utils::logger
