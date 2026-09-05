#pragma once

#include <asio.hpp>

#include <deque>
#include <memory>
#include <string>

namespace chat::server {

/**
 * 每个客户端连接对应一个 Session：
 * 负责按行读取消息、写回响应，并管理连接生命周期。
 * 通过 enable_shared_from_this 让异步回调持有自身，连接关闭后自动析构。
 */
class Session : public std::enable_shared_from_this<Session> {
private:
  Session(Session &) = delete;
  Session(Session &&) = delete;
  Session &operator=(const Session &) = delete;
  Session &operator=(Session &&) = delete;

private:
  asio::ip::tcp::socket socket_;
  asio::streambuf read_buffer_;
  std::deque<std::string> write_queue_; // 待发送消息队列
  bool writing_ = false;                // 是否有写操作进行中

public:
  explicit Session(asio::ip::tcp::socket socket);
  ~Session();

  void start();
  void deliver(const std::string &message); // 发送一行消息（自动追加换行）

private:
  void do_read();
  void do_write();
};

} // namespace chat::server
