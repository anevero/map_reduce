#ifndef BUFFERED_WRITER_H_
#define BUFFERED_WRITER_H_

#include <fstream>
#include <sstream>
#include <string>

class BufferedWriter {
 public:
  BufferedWriter() = default;
  explicit BufferedWriter(const std::string& dst_file);
  ~BufferedWriter();

  void Open(const std::string& dst_file);
  void Close();
  void Flush();

  template<class T>
  void WriteIntegralNumber(T number);
  void WriteChar(char c);
  void WriteString(const std::string& string);
  void WriteCStyleString(const char* string);

  template<class T>
  BufferedWriter& operator<<(T number);
  BufferedWriter& operator<<(char c);
  BufferedWriter& operator<<(const std::string& string);
  BufferedWriter& operator<<(const char* string);

 private:
  std::ofstream output_stream_;
  std::ostringstream cache_;

  int cache_length_ = 0;
};

template<class T>
void BufferedWriter::WriteIntegralNumber(T number) {
  static_assert(std::is_integral_v<T>);
  WriteString(std::to_string(number));
}

template<class T>
BufferedWriter& BufferedWriter::operator<<(T number) {
  static_assert(std::is_integral_v<T>);
  WriteIntegralNumber(number);
  return *this;
}

#endif  // BUFFERED_WRITER_H_
