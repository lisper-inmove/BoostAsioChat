#include "utils/logger.h"
#include <server/chat_server.h>
#include <server/config.h>
#include <spdlog/spdlog.h>

namespace chat::server {
Server::Server() { LOG_INFO("Server initialized..."); }
Server::~Server() { LOG_INFO("Server initialized..."); }

} // namespace chat::server
