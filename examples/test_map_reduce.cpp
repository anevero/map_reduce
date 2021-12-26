#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "absl/random/random.h"
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include "data_piece.pb.h"

#include "buffered_io/buffered_reader.h"
#include "buffered_io/buffered_writer.h"

namespace bf = boost::filesystem;
namespace bp = boost::process;

std::vector<DataPiece> GenerateTestData(int users_number = 2000,
                                        int entries_number = 2'000'000) {
  std::vector<std::string> sites_addresses =
      {"bsu.by", "fpmi.bsu.by", "student.bsu.by", "edufpmi.bsu.by",
       "webmail.bsu.by", "elib.bsu.by", "library.bsu.by",
       "acm.bsu.by", "lyceum.by", "courses.lyceum.by"};

  std::vector<DataPiece> result;
  result.reserve(entries_number);

  absl::BitGen random_generator;
  for (int i = 0; i < entries_number; ++i) {
    uint64_t user_id = absl::Uniform<uint64_t>(
        absl::IntervalClosed, random_generator, 1, users_number);
    int site_id = absl::Uniform<int>(
        absl::IntervalClosed, random_generator, 0, sites_addresses.size() - 1);
    uint64_t time = absl::Uniform<int>(
        absl::IntervalClosed, random_generator, 1, 10000);
    DataPiece entry;
    entry.set_key(sites_addresses[site_id]);
    entry.set_subkey(std::to_string(user_id));
    entry.set_value(std::to_string(time));
    result.push_back(std::move(entry));
  }

  return result;
}

void WriteTestData(const std::vector<DataPiece>& entries) {
  BufferedWriter writer{"./test_input.bin"};
  for (auto& entry: entries) {
    writer << entry;
  }
}

void RunMapStage() {
  auto process = bp::child{"./mapreduce", "map", "./map", "./test_input.bin",
                           "./test_map_output.bin"};
  process.wait();
}

void RunReduceStage() {
  auto process =
      bp::child{"./mapreduce", "reduce", "./reduce", "./test_map_output.bin",
                "./test_output.bin"};
  process.wait();
}

std::map<uint64_t, std::map<std::string, uint64_t>> GetExpectedResult(
    const std::vector<DataPiece>& input) {
  std::map<uint64_t, std::map<std::string, uint64_t>> result;
  for (auto& entry: input) {
    result[std::stoull(entry.subkey())][entry.key()] +=
        std::stoull(entry.value());
  }
  return result;
}

bool CheckResult(
    const std::map<uint64_t, std::map<std::string, uint64_t>>& expected) {
  std::map<uint64_t, std::map<std::string, uint64_t>> actual;
  BufferedReader reader{"./test_output.bin"};
  DataPiece entry;
  while (true) {
    entry = reader.ReadDataPiece();
    if (entry.key().empty()) {
      break;
    }
    actual[std::stoull(entry.key())][entry.subkey()] +=
        std::stoull(entry.value());
  }

  if (actual == expected) {
    return true;
  }

  std::ofstream actual_output("actual.txt");
  for (auto&&[user_id, history]: actual) {
    for (auto&&[site, time]: history) {
      actual_output << user_id << '\t' << site << '\t' << time << '\n';
    }
  }

  std::ofstream expected_output("expected.txt");
  for (auto&&[user_id, history]: expected) {
    for (auto&&[site, time]: history) {
      expected_output << user_id << '\t' << site << '\t' << time << '\n';
    }
  }

  return false;
}

void RemoveFiles() {
  bf::remove("./test_output.bin");
  bf::remove("./test_map_output.bin");
  bf::remove("./test_input.bin");
}

int main() {
  std::cout << "Generating test data" << std::endl;
  auto input = GenerateTestData();

  std::cout << "Writing test data to the file" << std::endl;
  WriteTestData(input);

  std::cout << "Running Map stage" << std::endl;
  RunMapStage();

  std::cout << "Running Reduce stage" << std::endl;
  RunReduceStage();

  std::cout << "Generating expected results" << std::endl;
  auto expected = GetExpectedResult(input);

  std::cout << "Comparing expected and actual results" << std::endl;
  bool equal = CheckResult(expected);

  if (equal) {
    std::cout << "Results are equal" << std::endl;
  } else {
    std::cout << "Results are NOT equal.\n"
              << "Expected and actual outputs are written to the files"
              << std::endl;
  }

  std::cout << "Removing temporary files and exiting" << std::endl;
  RemoveFiles();

  return 0;
}
