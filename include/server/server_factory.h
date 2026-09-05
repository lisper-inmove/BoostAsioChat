#pragma once

#include <memory>
#include <string>

namespace chat::server {
class Server;

/**
 * 服务器工厂（简单工厂模式）。
 * 根据配置中的 server.type 字符串创建对应的 Server 实现：
 *   - "ASIO"        -> ASIOServer
 *   - "PureSocket"  -> PureSocketServer
 */
class ServerFactory {
public:
  static std::unique_ptr<Server> create(const std::string &type);
};
} // namespace chat::server
