#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

const char KEY_VALUE_DELIMITER = '\t';
const char LINES_DELIMITER = '\n';

}  // namespace

std::vector<std::pair<std::string, int>> ReadPairs() {
  std::vector<std::pair<std::string, int>> pairs;
  std::string current_line;

  while (std::getline(std::cin, current_line, LINES_DELIMITER)) {
    std::string key;
    std::string value;

    std::istringstream line_stream(current_line);
    std::getline(line_stream, key, KEY_VALUE_DELIMITER);
    while (std::getline(line_stream, value, ' ')) {
      pairs.emplace_back(value, 1);
    }
  }

  return pairs;
}

void WritePairs(const std::vector<std::pair<std::string, int>>& pairs) {
  for (auto&&[key, value] : pairs) {
    std::cout << key << KEY_VALUE_DELIMITER << value << LINES_DELIMITER;
  }
}

int main() {
  WritePairs(ReadPairs());

  return 0;
}
