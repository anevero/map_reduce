// Input format (delimited protobufs):
// Key: site string
// Subkey: user_id
// Value: time

// Output format (delimited protobufs):
// Key: user_id
// Subkey: site string
// Value: time

// The first argument to the script should be the name of the input file, the
// second - the name of the output file.

#include <string>
#include <utility>
#include <vector>

#include "absl/random/random.h"
#include "data_piece.pb.h"

#include "buffered_io/buffered_reader.h"
#include "buffered_io/buffered_writer.h"
#include "utils/utils.h"

std::vector<DataPiece> ReadAndConvertEntries(const std::string& src_file) {
  std::vector<DataPiece> entries;
  BufferedReader reader(src_file);

  while (true) {
    auto data_piece = reader.ReadDataPiece();
    if (data_piece.key().empty()) {
      break;
    }
    std::swap(*data_piece.mutable_key(), *data_piece.mutable_subkey());
    entries.push_back(std::move(data_piece));
  }

  return entries;
}

void WriteEntries(const std::vector<DataPiece>& entries,
                  const std::string& dst_file) {
  BufferedWriter writer(dst_file);
  for (auto& entry: entries) {
    writer << entry;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Not enough arguments for this program." << std::endl;
    return 1;
  }

  if (auto status = utils::ValidateFile(argv[1]); !status.ok()) {
    std::cerr << status << std::endl;
    return 1;
  }

  absl::BitGen random_generator;
  if (absl::Uniform<uint64_t>(
      absl::IntervalClosed, random_generator, 1, 7) == 1) {
    return 1;
  }

  WriteEntries(ReadAndConvertEntries(argv[1]), argv[2]);

  return 0;
}
