#include "buffered_writer.h"
#include "constants.h"

#include <cstring>
#include <iostream>

BufferedWriter::BufferedWriter(const std::string& dst_file)
    : output_stream_(dst_file) {}

BufferedWriter::~BufferedWriter() {
  Close();
}

void BufferedWriter::Open(const std::string& dst_file) {
  output_stream_.open(dst_file);
}

void BufferedWriter::Close() {
  if (output_stream_.is_open()) {
    Flush();
    output_stream_.close();
  }
}

void BufferedWriter::Flush() {
  output_stream_ << cache_.str();
  cache_.str(std::string{});
  cache_length_ = 0;
}

void BufferedWriter::WriteChar(char c) {
  ++cache_length_;
  cache_ << c;

  if (cache_length_ >= kWriteBufferLength) {
    Flush();
  }
}

void BufferedWriter::WriteString(const std::string& string) {
  cache_length_ += string.length();
  cache_ << string;

  if (cache_length_ >= kWriteBufferLength) {
    Flush();
  }
}

void BufferedWriter::WriteCStyleString(const char* string) {
  cache_length_ += std::strlen(string);
  cache_ << string;

  if (cache_length_ >= kWriteBufferLength) {
    Flush();
  }
}

BufferedWriter& BufferedWriter::operator<<(char c) {
  WriteChar(c);
  return *this;
}

BufferedWriter& BufferedWriter::operator<<(const std::string& string) {
  WriteString(string);
  return *this;
}

BufferedWriter& BufferedWriter::operator<<(const char* string) {
  WriteCStyleString(string);
  return *this;
}
