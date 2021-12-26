#include "buffered_io/buffered_writer.h"
#include "utils/constants.h"

#include "google/protobuf/util/delimited_message_util.h"

BufferedWriter::BufferedWriter(const std::string& dst_file) {
  Open(dst_file);
}

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
}

void BufferedWriter::WriteMessage(const DataPiece& message) {
  google::protobuf::util::SerializeDelimitedToOstream(message, &cache_);
  if (cache_.tellg() >= constants::kWriteBufferLength) {
    Flush();
  }
}

BufferedWriter& BufferedWriter::operator<<(const DataPiece& message) {
  WriteMessage(message);
  return *this;
}
