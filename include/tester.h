#pragma once

#include <gtest/gtest.h>

namespace chat::tester {
class Tester : public ::testing::Test {
public:
  void SetUp() override;
  void TearDown() override;
};
} // namespace chat::tester
