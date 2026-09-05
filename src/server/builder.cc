#include <server/builder.h>
#include <server/config.h>
#include <yaml-cpp/yaml.h>

namespace chat::server::builder {
bool Builder::initConfig() {

  YAML::Node root = YAML::LoadFile("config.yaml");
  initLog(root);

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

bool Builder::initServer(const YAML::Node &root) { return true; }
} // namespace chat::server::builder
