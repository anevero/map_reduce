#include "buffered_io/buffered_reader.h"
#include "utils/constants.h"

#include <utility>

#include "google/protobuf/util/delimited_message_util.h"

BufferedReader::BufferedReader(const std::string& src_file) {
  Open(src_file);
}

void BufferedReader::Open(const std::string& src_file) {
  Close();
  file_stream_ = std::ifstream{src_file};
  input_stream_ = std::make_unique<google::protobuf::io::IstreamInputStream>(
      &file_stream_);
}

void BufferedReader::Close() {
  if (input_stream_ != nullptr) {
    input_stream_.reset(nullptr);
    file_stream_.close();
    cache_.clear();
  }
}

DataPiece BufferedReader::ReadDataPiece() {
  if (cache_.empty()) {
    Buffer();
  }
  if (cache_.empty()) {
    return DataPiece{};
  }
  auto value_to_return = std::move(cache_.front());
  cache_.pop_front();
  return value_to_return;
}

void BufferedReader::Buffer() {
  DataPiece message;
  while (cache_.size() < constants::kReadBufferNumberOfLines &&
      google::protobuf::util::ParseDelimitedFromZeroCopyStream(
          &message, input_stream_.get(), nullptr)) {
    cache_.push_back(std::move(message));
  }
}
