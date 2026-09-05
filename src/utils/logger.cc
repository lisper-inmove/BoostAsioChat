#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <utils/logger.h>
#include <vector>

namespace chat::utils::logger {
bool initLogger(const server::config::LogConfig &config) {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  std::unordered_map<std::string, spdlog::level::level_enum> lvlMap = {
      {"trace", spdlog::level::trace}, {"debug", spdlog::level::debug},
      {"info", spdlog::level::info},   {"warn", spdlog::level::warn},
      {"err", spdlog::level::err},     {"critical", spdlog::level::critical},
      {"off", spdlog::level::off},
  };
  console_sink->set_level(spdlog::level::trace);
  console_sink->set_pattern(config.getPattern());
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      config.getFile(), config.getMaxSize(), config.getMaxFiles());

  file_sink->set_level(lvlMap.at(config.getLevel()));
  file_sink->set_pattern(config.getPattern());

  std::vector<spdlog::sink_ptr> sinks = {console_sink, file_sink};
  auto logger =
      std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
  logger->set_level(spdlog::level::trace);

  spdlog::register_logger(logger);
  spdlog::set_default_logger(logger);
  return true;
}
} // namespace chat::utils::logger
