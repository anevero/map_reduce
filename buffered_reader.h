#ifndef MAPREDUCE_BUFFERED_READER_H_
#define MAPREDUCE_BUFFERED_READER_H_

#include <deque>
#include <fstream>
#include <string>

class BufferedReader {
 public:
  BufferedReader() = default;
  explicit BufferedReader(const std::string& src_file);

  void Open(const std::string& src_file);
  void Close();

  bool ReadLine(std::string* string);
  void Buffer();

 private:
  std::ifstream input_stream_;
  std::deque<std::string> cache_;
};

#endif  // MAPREDUCE_BUFFERED_READER_H_
