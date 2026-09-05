#include <server/chat_server.h>
#include <server/config.h>
#include <utils/logger.h>

namespace chat::server {

PureSocketServer::PureSocketServer() {
  LOG_INFO("PureSocket Server inited...");
}

void PureSocketServer::start() {
  auto &config = config::ServerConfig::getInstance();
  LOG_INFO("PureSocket Server started at {}, {}...", config.getHost(),
           config.getPort());
}

void PureSocketServer::stop() { LOG_INFO("PureSocket Server stopped..."); }

void PureSocketServer::do_accept() {}

} // namespace chat::server
