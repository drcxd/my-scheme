#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  std::string line;
  while (true) {
    std::cout << ">>> ";
    std::getline(std::cin, line);
    std::cout << line << std::endl;
  }
  return 0;
}
