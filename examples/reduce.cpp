// Input format (delimited protobufs):
// Key: user_id
// Subkey: site string
// Value: time

// Output format (delimited protobufs):
// Key: user_id
// Subkey: site string
// Value: time spent (sum)

// The first argument to the script should be the name of the input file, the
// second - the name of the output file. All the entries in the input file must
// have the same user id!

#include <iostream>
#include <string>
#include <vector>

#include "absl/random/random.h"
#include "data_piece.pb.h"

#include "buffered_io/buffered_reader.h"
#include "buffered_io/buffered_writer.h"
#include "utils/utils.h"

std::vector<DataPiece> ReadEntries(const std::string& src_file) {
  std::vector<DataPiece> entries;
  BufferedReader reader(src_file);

  while (true) {
    auto data_piece = reader.ReadDataPiece();
    if (data_piece.key().empty()) {
      break;
    }
    entries.push_back(std::move(data_piece));
  }

  return entries;
}

void ProcessAndWriteEntries(const std::vector<DataPiece>& entries,
                            const std::string& dst_file) {
  std::string user_id = entries.front().key();
  std::map<std::string, uint64_t> site_map;

  for (auto& entry: entries) {
    site_map[entry.subkey()] += std::stoull(entry.value());
  }

  BufferedWriter writer(dst_file);
  for (auto&&[site_address, time]: site_map) {
    DataPiece entry;
    entry.set_key(user_id);
    entry.set_subkey(site_address);
    entry.set_value(std::to_string(time));
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

  ProcessAndWriteEntries(ReadEntries(argv[1]), argv[2]);

  return 0;
}
