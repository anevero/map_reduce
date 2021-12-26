#ifndef BUFFERED_READER_H_
#define BUFFERED_READER_H_

#include <deque>
#include <fstream>
#include <memory>
#include <string>

#include "google/protobuf/io/zero_copy_stream_impl.h"

#include "data_piece.pb.h"

class BufferedReader {
 public:
  BufferedReader() = default;
  explicit BufferedReader(const std::string& src_file);

  void Open(const std::string& src_file);
  void Close();

  DataPiece ReadDataPiece();
  void Buffer();

 private:
  std::ifstream file_stream_;
  std::unique_ptr<google::protobuf::io::IstreamInputStream>
      input_stream_ = nullptr;
  std::deque<DataPiece> cache_;
};

#endif  // BUFFERED_READER_H_
