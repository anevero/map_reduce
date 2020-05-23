#include "buffered_reader.h"
#include "constants.h"

BufferedReader::BufferedReader(const std::string& src_file)
    : input_stream_(src_file) {}

void BufferedReader::Open(const std::string& src_file) {
  Close();
  input_stream_.open(src_file);
}

void BufferedReader::Close() {
  if (input_stream_.is_open()) {
    input_stream_.close();
    cache_.clear();
  }
}

bool BufferedReader::ReadLine(std::string* string) {
  if (cache_.empty()) {
    Buffer();
  }

  if (cache_.empty()) {
    return false;
  }

  *string = std::move(cache_.front());
  cache_.pop_front();
  return true;
}

void BufferedReader::Buffer() {
  std::string current_line;
  while (cache_.size() < kReadBufferNumberOfLines
      && std::getline(input_stream_, current_line, kLinesDelimiter)) {
    cache_.push_back(std::move(current_line));
  }
}
