#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <utils/logger.h>
#include <vector>

namespace chat::utils::logger {
bool initLogger(const server::config::LogConfig &config) {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::trace);
  console_sink->set_pattern(config.getPattern());
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      config.getFile(), config.getMaxSize(), config.getMaxFiles());

  file_sink->set_level(spdlog::level_from_str(config.getLevel()));
  file_sink->set_pattern(config.getPattern());

  std::vector<spdlog::sink_ptr> sinks = {console_sink, file_sink};
  auto logger =
      std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
  logger->set_level(spdlog::level::trace);

  spdlog::set_global_logger(logger);
  return true;
}
} // namespace chat::utils::logger
