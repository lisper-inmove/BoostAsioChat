#include "gtest/gtest.h"
#include <iostream>
#include <tester.h>
#include <yaml-cpp/yaml.h>

namespace chat::tester {
TEST_F(Tester, YAML_Cpp_Tester) {
  YAML::Node config = YAML::LoadFile("config/config.yaml");
  YAML::Node host_node = config["server"]["host"];
  YAML::Node port_node = config["server"]["port"];
  std::cout << "host " << host_node.as<std::string>() << " port "
            << port_node.as<uint16_t>() << "\n";
  EXPECT_EQ(host_node.as<std::string>(), "127.0.0.1");
  EXPECT_EQ(port_node.as<int16_t>(), 9527);
}
} // namespace chat::tester
