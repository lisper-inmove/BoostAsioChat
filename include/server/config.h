#pragma once

#include <cstdint>
#include <memory>
#include <string>
namespace chat::server::config {
class LogConfig {
private:
  std::string file_{""};
  std::string level_{"info"};
  std::uint32_t max_size_{104857600};
  std::uint32_t max_files_{100};
  std::string pattern_{"[%Y-%m-%d %H:%M:%S.%e] [%l] %v"};

public:
  void setFile(std::string &&file) { file_ = file; }
  void setLevel(std::string &&level) { level_ = level; }
  void setMaxSize(std::uint32_t max_size) { max_size_ = max_size; }
  void setMaxFiles(std::uint32_t max_files) { max_files_ = max_files; }
  void setPattern(std::string &&pattern) { pattern_ = pattern; }

  [[nodiscard]] const std::string getFile() const { return file_; }
  [[nodiscard]] const std::string getLevel() const { return level_; }
  [[nodiscard]] const std::uint32_t getMaxSize() const { return max_size_; }
  [[nodiscard]] const std::uint32_t getMaxFiles() const { return max_files_; }
  [[nodiscard]] const std::string getPattern() const { return pattern_; }
};

class ServerConfig {
private:
  ServerConfig() = default;
  ~ServerConfig() = default;
  ServerConfig(const ServerConfig &) = delete;
  ServerConfig(ServerConfig &&) = delete;
  ServerConfig &operator=(const ServerConfig &) = delete;
  ServerConfig &operator=(ServerConfig &&) = delete;

public:
  static ServerConfig &getInstance() {
    static ServerConfig instance;
    return instance;
  }

private:
  std::string host_{"127.0.0.1"};
  std::uint16_t port_{9527};
  std::unique_ptr<LogConfig> logConfig_ = nullptr;

public:
  LogConfig &getLogConfig();
};

} // namespace chat::server::config
