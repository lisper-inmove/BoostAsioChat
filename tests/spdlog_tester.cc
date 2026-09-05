#include <tester.h>

#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <sstream>
#include <string>

namespace chat::tester {

// 冒烟测试：验证 spdlog（header-only）+ 系统 fmt 的格式化输出链路可用
TEST_F(Tester, Spdlog_Formats_And_Logs) {
  std::ostringstream oss;
  auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
  auto logger = std::make_shared<spdlog::logger>("spdlog_tester", sink);
  logger->set_pattern("[%n] %v");

  logger->info("hello {} #{:03d}", "chat", 7);
  logger->flush();

  EXPECT_NE(oss.str().find("[spdlog_tester] hello chat #007"),
            std::string::npos);
}

} // namespace chat::tester
