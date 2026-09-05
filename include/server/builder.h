#pragma once

#include <yaml-cpp/yaml.h>

namespace chat::server::builder {
class Builder {
public:
  bool initConfig();
  bool initLog(const YAML::Node &);
  bool initServer(const YAML::Node &);
};
} // namespace chat::server::builder
