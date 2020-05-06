#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "constants.h"

std::pair<std::string, int> ReadPairs() {
  std::pair<std::string, int> pair{};
  std::string current_line;

  while (std::getline(std::cin, current_line, kLinesDelimiter)) {
    std::string key;  // must always be the same
    std::string value;  // always equal to 1

    std::istringstream line_stream(current_line);
    std::getline(line_stream, key, kKeyValueDelimiter);
    std::getline(line_stream, value, kLinesDelimiter);

    if (!pair.first.empty() && key != pair.first) {
      std::cerr << "Weird number of keys. Check the source file structure.\n";
      return {};
    }
    if (pair.first.empty()) {
      pair.first = std::move(key);
    }

    ++pair.second;
  }

  return pair;
}

void WritePair(const std::pair<std::string, int>& pair) {
  std::cout << pair.first << kKeyValueDelimiter << pair.second
            << kLinesDelimiter;
}

int main() {
  WritePair(ReadPairs());

  return 0;
}
