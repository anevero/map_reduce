#include "mapreduce_lib/temp_files_manager.h"
#include "buffered_io/buffered_reader.h"
#include "buffered_io/buffered_writer.h"
#include "utils/constants.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>

#include "data_piece.pb.h"
#include "mapreduce_lib/process_manager.h"
#include "mapreduce_lib/threadpool.h"

namespace bf = boost::filesystem;

TempFilesManager::~TempFilesManager() {
  RemoveTemporaryFiles();
}

void TempFilesManager::CreateTemporaryFilesByBlockSize(
    const std::string& src_file) {
  BufferedReader reader(src_file);
  BufferedWriter writer;

  int lines_in_current_file = constants::kNumberOfLinesInBlock;
  DataPiece entry;

  while (true) {
    entry = reader.ReadDataPiece();
    if (entry.key().empty()) {
      break;
    }

    if (lines_in_current_file >= constants::kNumberOfLinesInBlock) {
      writer.Close();
      CreateTemporaryFile();
      writer.Open(temp_files_.back());
      lines_in_current_file = 0;
    }

    writer << entry;
    ++lines_in_current_file;
  }
}

void TempFilesManager::CreateTemporaryFilesByKeys(
    const std::string& src_file) {
  BufferedReader reader(src_file);
  BufferedWriter writer;

  std::string previous_key = {};
  DataPiece entry;

  while (true) {
    entry = reader.ReadDataPiece();
    if (entry.key().empty()) {
      break;
    }

    if (entry.key() != previous_key) {
      writer.Close();
      CreateTemporaryFile();
      writer.Open(temp_files_.back());
      previous_key = entry.key();
    }

    writer << entry;
  }
}

absl::Status TempFilesManager::RunScriptOnTemporaryFiles(
    const std::string& script_path) {
  ProcessManager process_manager(temp_files_);
  return process_manager.RunAndWait(script_path);
}

void TempFilesManager::SortLinesInTemporaryFiles() {
  ThreadPool thread_pool(constants::kNumberOfThreadsMultiplier *
      std::thread::hardware_concurrency());
  for (const auto& file: temp_files_) {
    thread_pool.Schedule([this, &file] { SortTemporaryFile(file); });
  }
  thread_pool.StartWorkers();
}

void TempFilesManager::MergeTemporaryFiles(
    const std::string& dst_file) const {
  std::ofstream out(dst_file, std::ios::binary);
  for (const auto& file: temp_files_) {
    std::ifstream in(file, std::ios_base::binary);
    out << in.rdbuf();
  }
}

void TempFilesManager::MergeSortedTemporaryFiles(
    const std::string& dst_file) const {
  int number_of_files = temp_files_.size();
  BufferedWriter writer(dst_file);
  std::vector<BufferedReader> readers(number_of_files);
  for (int i = 0; i < number_of_files; ++i) {
    readers[i].Open(temp_files_[i]);
  }

  struct HeapEntry {
    DataPiece entry;
    int file_number;

    bool operator<(const HeapEntry& other) const {
      if (entry.key() == other.entry.key()) {
        return entry.subkey() > other.entry.subkey();
      }
      return entry.key() > other.entry.key();
    }
  };

  std::vector<HeapEntry> heap;
  heap.reserve(number_of_files);
  for (int i = 0; i < number_of_files; ++i) {
    heap.push_back({readers[i].ReadDataPiece(), i});
  }
  std::make_heap(heap.begin(), heap.end());

  while (!heap.empty()) {
    writer << heap.front().entry;
    std::pop_heap(heap.begin(), heap.end());
    auto new_entry = readers[heap.back().file_number].ReadDataPiece();
    if (new_entry.key().empty()) {
      heap.pop_back();
    } else {
      heap.back().entry = new_entry;
      std::push_heap(heap.begin(), heap.end());
    }
  }
}

void TempFilesManager::RemoveTemporaryFiles() {
  while (!temp_files_.empty()) {
    bf::remove(temp_files_.back());
    temp_files_.pop_back();
  }
}

void TempFilesManager::CreateTemporaryFile() {
  temp_files_.push_back(bf::unique_path().native());
  std::ofstream temp_stream(temp_files_.back());
}

void TempFilesManager::SortTemporaryFile(const std::string& file) {
  std::vector<DataPiece> entries;
  entries.reserve(constants::kNumberOfLinesInBlock);

  BufferedReader reader(file);
  while (true) {
    auto new_entry = reader.ReadDataPiece();
    if (new_entry.key().empty()) {
      break;
    }
    entries.push_back(std::move(new_entry));
  }
  reader.Close();

  std::sort(entries.begin(),
            entries.end(),
            [](const DataPiece& a, const DataPiece& b) {
              if (a.key() == b.key()) {
                return a.subkey() < b.subkey();
              }
              return a.key() < b.key();
            });

  BufferedWriter writer(file);
  for (const auto& entry: entries) {
    writer << entry;
  }
}
