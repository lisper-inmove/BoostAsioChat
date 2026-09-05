#pragma once

#include "asio/io_context.hpp"
#include <asio.hpp>

using asio::ip::tcp;

namespace chat::server {
class Server {
private:
  Server(Server &) = delete;
  Server(Server &&) = delete;
  Server &operator=(const Server &) = delete;
  Server &operator=(Server &&) = delete;

public:
  Server();
  virtual ~Server();
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void do_accept() = 0;
};

/**
 * 基于 ASIO 的服务端
 * */
class ASIOServer : public Server {

private:
  ASIOServer(ASIOServer &) = delete;
  ASIOServer(ASIOServer &&) = delete;
  ASIOServer &operator=(const ASIOServer &) = delete;
  ASIOServer &operator=(ASIOServer &&) = delete;

private:
  std::unique_ptr<tcp::acceptor> acceptor_ = nullptr;
  std::unique_ptr<asio::io_context> io_context_ = nullptr;

public:
  void start() override final;
  void stop() override final;
  void do_accept() override final;
  ASIOServer();
};

/**
 * 基于原始socket的服务端
 * */
class PureSocketServer : public Server {
private:
  PureSocketServer(PureSocketServer &) = delete;
  PureSocketServer(PureSocketServer &&) = delete;
  PureSocketServer &operator=(const PureSocketServer &) = delete;
  PureSocketServer &operator=(PureSocketServer &&) = delete;

public:
  void start() override final;
  void stop() override final;
  void do_accept() override final;
  PureSocketServer();
};
} // namespace chat::server
