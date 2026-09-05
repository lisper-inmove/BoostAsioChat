#include <server/config.h>

namespace chat::server::config {

LogConfig &ServerConfig::getLogConfig() noexcept {
  if (!logConfig_) {
    logConfig_ = std::make_unique<LogConfig>();
  }
  return *logConfig_;
}

} // namespace chat::server::config
