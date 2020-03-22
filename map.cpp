#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

const char KEY_VALUE_DELIMITER = '\t';
const char LINES_DELIMITER = '\n';

}  // namespace

std::vector<std::pair<std::string, int>> ReadPairs(
    const std::string& src_file) {
  std::vector<std::pair<std::string, int>> pairs;
  std::ifstream in(src_file, std::ios::binary);
  std::string current_line;

  while (std::getline(in, current_line, LINES_DELIMITER)) {
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

void WritePairs(const std::vector<std::pair<std::string, int>>& pairs,
                const std::string& dst_file) {
  std::ofstream out(dst_file, std::ios::binary);
  for (auto&&[key, value] : pairs) {
    out << key << KEY_VALUE_DELIMITER << value << LINES_DELIMITER;
  }
}

int main(int argc, char* argv[]) {
  (void) argc;
  std::string src_file = argv[1];
  std::string dst_file = argv[2];
  WritePairs(ReadPairs(src_file), dst_file);

  return 0;
}
