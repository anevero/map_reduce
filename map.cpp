#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "constants.h"

std::vector<std::string> ReadPairs() {
  std::vector<std::string> words;
  std::string current_line;

  while (std::getline(std::cin, current_line, kLinesDelimiter)) {
    std::string key;  // word "key" in this task
    std::string word;

    std::istringstream line_stream(current_line);
    std::getline(line_stream, key, kKeyValueDelimiter);
    while (std::getline(line_stream, word, kWordsDelimiter)) {
      words.push_back(std::move(word));
    }
  }

  return words;
}

void WritePairs(const std::vector<std::string>& words) {
  std::cin.tie(nullptr);
  std::ios::sync_with_stdio(false);

  for (auto& word : words) {
    std::cout << word << kKeyValueDelimiter << 1 << kLinesDelimiter;
  }

  std::cin.tie(&std::cout);
  std::ios::sync_with_stdio(true);
}

int main() {
  WritePairs(ReadPairs());

  return 0;
}
