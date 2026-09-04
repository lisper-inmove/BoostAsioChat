#include <asio.hpp>
#include <iostream>
#include <utils/datetime.h>

int main() {

  std::string now_str = chat::datetime::now_str();
  std::cout << now_str << "\n";

  return 0;
}
