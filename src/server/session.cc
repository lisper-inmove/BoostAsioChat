#include <server/session.h>
#include <utils/logger.h>

#include <iostream>
#include <system_error>

namespace chat::server {

Session::Session(asio::ip::tcp::socket socket) : socket_(std::move(socket)) {
  LOG_INFO("Session created.");
}

Session::~Session() { LOG_INFO("Session closed."); }

void Session::start() { do_read(); }

void Session::deliver(const std::string &message) {
  write_queue_.push_back(message + "\n");
  do_write();
}

void Session::do_read() {
  auto self = shared_from_this();
  asio::async_read_until(
      socket_, read_buffer_, '\n',
      [this, self](std::error_code ec, std::size_t /*bytes*/) {
        if (ec) {
          LOG_INFO("read ended: {}", ec.message());
          return; // self 释放，无其它引用时 Session 析构
        }

        std::istream is(&read_buffer_);
        std::string line;
        std::getline(is, line);
        if (!line.empty() && line.back() == '\r') {
          line.pop_back(); // 去掉 CRLF 的 \r
        }

        LOG_INFO("received: {}", line);
        deliver(line); // 回显（后续替换为消息路由 / 广播）

        do_read();
      });
}

void Session::do_write() {
  if (writing_ || write_queue_.empty()) {
    return;
  }
  writing_ = true;

  auto self = shared_from_this();
  auto msg = std::make_shared<std::string>(write_queue_.front());
  write_queue_.pop_front();

  asio::async_write(socket_, asio::buffer(*msg),
                    [this, self, msg](std::error_code ec, std::size_t) {
                      writing_ = false;
                      if (ec) {
                        LOG_INFO("write ended: {}", ec.message());
                        return;
                      }
                      do_write(); // 继续发送队列里的下一条
                    });
}

} // namespace chat::server
