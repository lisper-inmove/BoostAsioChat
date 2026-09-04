#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
namespace chat::datetime {
std::string format_time(const std::chrono::system_clock::time_point &tp,
                        const std::string &fmt = "%Y-%m-%d %H:%M:%S") {
  std::time_t tt = std::chrono::system_clock::to_time_t(tp);
  std::tm tm = *std::localtime(&tt);
  std::ostringstream oss;
  oss << std::put_time(&tm, fmt.c_str());
  return oss.str();
}

std::string now_str() { return format_time(std::chrono::system_clock::now()); }

} // namespace chat::datetime
