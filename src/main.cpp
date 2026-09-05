#include "server/config.h"
#include <asio.hpp>
#include <server/builder.h>
#include <spdlog/spdlog.h>
#include <utils/datetime.h>
#include <utils/logger.h>

int main() {

  std::string now_str = chat::datetime::now_str();
  chat::server::builder::Builder().initConfig();
  auto &config = chat::server::config::ServerConfig::getInstance();
  chat::utils::logger::initLogger(config.getLogConfig());

  return 0;
}
