#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>

namespace chat::server {
class Server;
}

namespace chat::server::builder {
class Builder {
public:
  Builder() = default;
  ~Builder();

  bool initConfig();
  bool initLog(const YAML::Node &);
  bool initServer(const YAML::Node &);

  chat::server::Server *getServer() const { return server_.get(); }

private:
  std::unique_ptr<chat::server::Server> server_;
};
} // namespace chat::server::builder
