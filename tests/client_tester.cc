#include <tester.h>

#include <asio.hpp>
#include <server/chat_server.h>
#include <server/config.h>

#include <chrono>
#include <iostream>
#include <thread>

namespace chat::tester {

// 客户端测试：后台启动 ASIOServer，连接后发送 hello，验证收到服务端回显
TEST_F(Tester, Client_Connect_Tester) {
  using asio::ip::tcp;

  const std::string host = "127.0.0.1";
  const std::uint16_t port = 19527; // 独立测试端口，避免与真实服务冲突

  // 配置服务端监听测试端口
  auto &config = chat::server::config::ServerConfig::getInstance();
  config.setHost(host);
  config.setPort(port);

  // 后台线程启动服务端
  chat::server::ASIOServer server;
  std::jthread server_thread([&server] {
    try {
      server.start();
    } catch (const std::exception &e) {
      std::cerr << "server start failed: " << e.what() << "\n";
    }
  });

  // 客户端重试连接（服务端 bind + listen 需要一点时间）
  asio::io_context io;
  std::error_code ec;
  tcp::socket socket(io);
  bool connected = false;
  for (int i = 0; i < 20 && !connected; ++i) {
    tcp::socket tmp(io);
    tmp.connect(tcp::endpoint(asio::ip::make_address(host), port), ec);
    if (!ec) {
      socket = std::move(tmp);
      connected = true;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  ASSERT_TRUE(connected) << "client connect failed: " << ec.message();

  // 发送 hello
  std::string msg = "hello\n";
  asio::write(socket, asio::buffer(msg), ec);
  ASSERT_FALSE(ec) << "client write failed: " << ec.message();

  // 接收服务端回显（带超时）
  asio::streambuf read_buf;
  bool received = false;
  asio::async_read_until(socket, read_buf, '\n',
                         [&received](std::error_code e, std::size_t) {
                           received = !e;
                         });
  io.run_for(std::chrono::milliseconds(500));
  ASSERT_TRUE(received) << "no echo received (timeout)";

  std::istream is(&read_buf);
  std::string echoed;
  std::getline(is, echoed);
  EXPECT_EQ(echoed, "hello");

  socket.close();
  server.stop();
  server_thread.join();
}

} // namespace chat::tester
