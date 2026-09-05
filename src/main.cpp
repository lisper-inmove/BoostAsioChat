#include <atomic>
#include <chrono>
#include <csignal>
#include <server/builder.h>
#include <server/chat_server.h>
#include <server/config.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <utils/logger.h>

namespace {
std::atomic<bool> g_stop{false};

void signal_handler(int) { g_stop.store(true); }
} // namespace

int main() {
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  chat::server::builder::Builder builder;
  builder.initConfig();

  auto &config = chat::server::config::ServerConfig::getInstance();
  chat::utils::logger::initLogger(config.getLogConfig());

  auto *server = builder.getServer();
  if (!server) {
    return 1;
  }

  std::jthread server_thread([server] { server->start(); });

  // 主线程等待退出信号（Ctrl+C / kill）
  while (!g_stop.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  spdlog::info("shutdown signal received, stopping server...");
  server->stop();

  // server_thread 析构时自动 join，等待 start() 返回
  return 0;
}
