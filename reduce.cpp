#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace {

const char KEY_VALUE_DELIMITER = '\t';
const char LINES_DELIMITER = '\n';

}  // namespace

std::unordered_map<std::string, int> ReadPairs() {
  std::unordered_map<std::string, int> pairs;
  std::string current_line;

  while (std::getline(std::cin, current_line, LINES_DELIMITER)) {
    std::string key;
    std::string value;

    std::istringstream line_stream(current_line);
    std::getline(line_stream, key, KEY_VALUE_DELIMITER);
    std::getline(line_stream, value, LINES_DELIMITER);

    pairs[key] += std::stoi(value);
  }

  if (pairs.size() > 1) {
    std::clog << "Weird number of keys. Check the source file structure.\n";
  }

  return pairs;
}

void WritePairs(const std::unordered_map<std::string, int>& pairs) {
  for (auto&&[key, value] : pairs) {
    std::cout << key << KEY_VALUE_DELIMITER << value << LINES_DELIMITER;
  }
}

int main() {
  WritePairs(ReadPairs());

  return 0;
}
