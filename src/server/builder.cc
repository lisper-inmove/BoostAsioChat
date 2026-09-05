#include <server/builder.h>
#include <server/chat_server.h>
#include <server/config.h>
#include <server/server_factory.h>
#include <yaml-cpp/yaml.h>

namespace chat::server::builder {

Builder::~Builder() = default;

bool Builder::initConfig() {

  YAML::Node root = YAML::LoadFile("config.yaml");
  initLog(root);
  initServer(root);

  return true;
}

bool Builder::initLog(const YAML::Node &root) {
  auto &config = config::ServerConfig::getInstance();
  config::LogConfig &log_config = config.getLogConfig();
  YAML::Node log_node = root["log"];
  // 日志保存位置
  auto file_node = log_node["file"];
  auto level_node = log_node["level"];
  auto max_size_node = log_node["max_size"];
  auto max_files_node = log_node["max_files"];
  auto pattern_node = log_node["pattern"];
  log_config.setFile(file_node.as<std::string>());
  log_config.setLevel(level_node.as<std::string>());
  log_config.setMaxSize(max_size_node.as<std::uint32_t>());
  log_config.setMaxFiles(max_files_node.as<uint32_t>());
  log_config.setPattern(pattern_node.as<std::string>());
  return true;
}

bool Builder::initServer(const YAML::Node &root) {
  auto &config = config::ServerConfig::getInstance();
  YAML::Node server_node = root["server"];
  auto host_node = server_node["host"];
  auto port_node = server_node["port"];
  auto type_node = server_node["type"];
  config.setHost(host_node.as<std::string>());
  config.setPort(port_node.as<uint16_t>());
  config.setType(type_node.as<std::string>());

  server_ = ServerFactory::create(config.getType());
  return server_ != nullptr;
}

} // namespace chat::server::builder
