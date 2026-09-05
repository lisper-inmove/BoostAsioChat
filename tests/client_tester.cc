#include <tester.h>

#include <asio.hpp>
#include <server/chat_server.h>
#include <server/config.h>

#include <chrono>
#include <iostream>
#include <thread>

namespace chat::tester {

// 客户端测试：后台启动 ASIOServer，用 asio 客户端连接并验证连接成功
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
  bool connected = false;
  for (int i = 0; i < 20 && !connected; ++i) {
    tcp::socket socket(io);
    socket.connect(tcp::endpoint(asio::ip::make_address(host), port), ec);
    if (!ec) {
      connected = true;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  EXPECT_TRUE(connected) << "client connect failed: " << ec.message();

  // 给服务端一点时间处理 accept（触发 do_accept 回调）
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  server.stop();
  server_thread.join();
}

} // namespace chat::tester
