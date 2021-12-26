#ifndef BUFFERED_WRITER_H_
#define BUFFERED_WRITER_H_

#include <fstream>
#include <sstream>
#include <string>

#include "google/protobuf/io/zero_copy_stream_impl.h"

#include "data_piece.pb.h"

class BufferedWriter {
 public:
  BufferedWriter() = default;
  explicit BufferedWriter(const std::string& dst_file);
  ~BufferedWriter();

  void Open(const std::string& dst_file);
  void Close();
  void Flush();

  void WriteMessage(const DataPiece& message);
  BufferedWriter& operator<<(const DataPiece& message);

 private:
  std::ofstream output_stream_;
  std::stringstream cache_;
};

#endif  // BUFFERED_WRITER_H_
