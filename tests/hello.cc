#include <iostream>
#include <tester.h>
#include <utils/datetime.h>

namespace chat::tester {
TEST_F(Tester, Hello_Tester) { std::cout << "Hello Chat\n"; }

TEST_F(Tester, Now_Str_Tester) {
  std::string now_str = chat::datetime::now_str();
  std::cout << "Hello Chat " << now_str << "\n";
}
} // namespace chat::tester
