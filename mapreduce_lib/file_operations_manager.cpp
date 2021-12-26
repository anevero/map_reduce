#include "mapreduce_lib/file_operations_manager.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include "data_piece.pb.h"

#include "buffered_io/buffered_reader.h"
#include "buffered_io/buffered_writer.h"
#include "mapreduce_lib/threadpool.h"
#include "utils/constants.h"

namespace bf = boost::filesystem;

FileOperationsManager::~FileOperationsManager() {
  RemoveAllTemporaryFiles();
}

std::string FileOperationsManager::GetFilenameById(int file_id) const {
  return temp_files_.at(file_id);
}

std::vector<int> FileOperationsManager::DivideFileByBlockSize(
    const std::string& src_file) {
  BufferedReader reader(src_file);
  BufferedWriter writer;
  std::vector<int> result_files_ids;

  int lines_in_current_file = constants::kNumberOfLinesInBlock;
  DataPiece entry;

  while (true) {
    entry = reader.ReadDataPiece();
    if (entry.key().empty()) {
      break;
    }

    if (lines_in_current_file >= constants::kNumberOfLinesInBlock) {
      writer.Close();

      int new_file_id = CreateTemporaryFile();
      result_files_ids.push_back(new_file_id);

      writer.Open(GetFilenameById(new_file_id));
      lines_in_current_file = 0;
    }

    writer << entry;
    ++lines_in_current_file;
  }

  return result_files_ids;
}

std::vector<int> FileOperationsManager::DivideFileByKey(
    const std::string& src_file) {
  BufferedReader reader(src_file);
  BufferedWriter writer;
  std::vector<int> result_files_ids;

  std::string previous_key = {};
  DataPiece entry;

  while (true) {
    entry = reader.ReadDataPiece();
    if (entry.key().empty()) {
      break;
    }

    if (entry.key() != previous_key) {
      writer.Close();

      int new_file_id = CreateTemporaryFile();
      result_files_ids.push_back(new_file_id);

      writer.Open(GetFilenameById(new_file_id));
      previous_key = entry.key();
    }

    writer << entry;
  }

  return result_files_ids;
}

void FileOperationsManager::MergeTemporaryFiles(
    const std::vector<int>& files_ids, const std::string& dst_file) {
  std::ofstream out(dst_file, std::ios::binary);
  for (int file_id: files_ids) {
    std::ifstream in(GetFilenameById(file_id), std::ios_base::binary);
    out << in.rdbuf();
    RemoveTemporaryFile(file_id);
  }
}

void FileOperationsManager::SortLinesInTemporaryFiles(
    const std::vector<int>& files_ids) const {
  ThreadPool thread_pool(constants::kNumberOfThreadsMultiplier *
      std::thread::hardware_concurrency());
  for (int file_id: files_ids) {
    thread_pool.Schedule([this, file_id] { SortTemporaryFile(file_id); });
  }
  thread_pool.StartWorkers();
}

void FileOperationsManager::MergeSortedTemporaryFiles(
    const std::vector<int>& files_ids, const std::string& dst_file) {
  int number_of_files = files_ids.size();
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

  RemoveTemporaryFiles(files_ids);
}

std::vector<int> FileOperationsManager::CreateTemporaryFiles(int count) {
  std::vector<int> files_ids;
  files_ids.reserve(count);
  for (int i = 0; i < count; ++i) {
    files_ids.push_back(CreateTemporaryFile());
  }
  return files_ids;
}

void FileOperationsManager::RemoveTemporaryFiles(
    const std::vector<int>& files_ids) {
  for (int file_id: files_ids) {
    RemoveTemporaryFile(file_id);
  }
}

void FileOperationsManager::RemoveAllTemporaryFiles() {
  while (!temp_files_.empty()) {
    RemoveTemporaryFile(temp_files_.begin()->first);
  }
}

int FileOperationsManager::CreateTemporaryFile() {
  int id = current_file_id_++;
  std::string filename = bf::unique_path().native();
  std::ofstream temp_stream(filename);
  temp_files_.emplace(id, std::move(filename));
  return id;
}

void FileOperationsManager::RemoveTemporaryFile(int file_id) {
  bf::remove(temp_files_.at(file_id));
  temp_files_.erase(file_id);
}

void FileOperationsManager::SortTemporaryFile(int file_id) const {
  std::vector<DataPiece> entries;
  entries.reserve(constants::kNumberOfLinesInBlock);

  BufferedReader reader(GetFilenameById(file_id));
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

  BufferedWriter writer(GetFilenameById(file_id));
  for (const auto& entry: entries) {
    writer << entry;
  }
}
