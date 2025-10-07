#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>

#include "Lexer.hh"

using namespace std;

void runPrompt() {
  std::string line;
  while (true) {
    std::cout << ">>> ";
    std::getline(std::cin, line);
    std::cout << line << std::endl;
  }
}

int runFile(std::string_view file) {
  std::ifstream fs{file.data(), std::ios_base::binary};
  if (fs.good()) {
    auto size = std::filesystem::file_size(file);
    std::string buffer(size, 0);
    fs.read(buffer.data(), size);

    Lexer lexer{buffer};
    auto tokens = lexer.getAllTokens();
    for (auto& token : tokens) {
      std::cout << toString(token) << std::endl;
    }
    return 0;
  }
  std::cerr << "Bad input file " << file << std::endl;
  return -1;
}

int main(int argc, char **argv) {
  int retCode = 0;
  if (argc < 2) {
    runPrompt();
  } else {
    std::string file{argv[1]};
    retCode = runFile(file);
  }
  return retCode;
}
