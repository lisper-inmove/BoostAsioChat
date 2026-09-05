#include <memory>
#include <server/chat_server.h>
#include <server/config.h>
#include <system_error>
#include <utils/logger.h>

namespace chat::server {

ASIOServer::ASIOServer() { LOG_INFO("ASIO Server initialized..."); }

void ASIOServer::start() {
  auto &config = config::ServerConfig::getInstance();
  const std::string host = config.getHost();
  const std::uint16_t port = config.getPort();
  LOG_INFO("ASIO Server started at {}, {}...", host, port);

  io_context_ = std::make_unique<asio::io_context>();
  acceptor_ = std::make_unique<tcp::acceptor>(
      *io_context_, tcp::endpoint(asio::ip::make_address(host), port));
  acceptor_->set_option(tcp::socket::reuse_address(true));

  do_accept();
  io_context_->run(); // 阻塞事件循环，直到 stop() 被调用
}

void ASIOServer::stop() {
  if (acceptor_) {
    acceptor_->close();
  }
  if (io_context_) {
    io_context_->stop();
  }
}

void ASIOServer::do_accept() {
  acceptor_->async_accept([this](std::error_code ec, tcp::socket socket) {
    if (ec) {
      LOG_ERR("accept stopped: {}", ec.message());
      return;
    }
    LOG_INFO("connect initialized.");
    do_accept();
  });
}

} // namespace chat::server
