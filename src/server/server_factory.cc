#include <server/chat_server.h>
#include <server/server_factory.h>
#include <spdlog/spdlog.h>
#include <utils/logger.h>

namespace chat::server {

std::unique_ptr<Server> ServerFactory::create(const std::string &type) {
  if (type == "ASIO") {
    return std::make_unique<ASIOServer>();
  }
  if (type == "PureSocket") {
    return std::make_unique<PureSocketServer>();
  }

  LOG_ERR("Unknown server type: {}", type);
  return nullptr;
}

} // namespace chat::server
