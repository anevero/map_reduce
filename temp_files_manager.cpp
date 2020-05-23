#include "buffered_reader.h"
#include "buffered_writer.h"
#include "constants.h"
#include "temp_files_manager.h"
#include "threadpool.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

TempFilesManager::~TempFilesManager() {
  RemoveTemporaryFiles();
}

void TempFilesManager::CreateTemporaryFilesByBlockSize(
    const std::string& src_file) {

  BufferedReader reader(src_file);
  BufferedWriter writer;

  int lines_in_current_file = kNumberOfLinesInBlock;
  std::string current_line;

  while (reader.ReadLine(&current_line)) {
    if (lines_in_current_file >= kNumberOfLinesInBlock) {
      writer.Close();
      CreateTemporaryFile();
      writer.Open(temp_files_.back());
      lines_in_current_file = 0;
    }

    writer << current_line << kLinesDelimiter;
    ++lines_in_current_file;
  }
}

void TempFilesManager::CreateTemporaryFilesByKeys(
    const std::string& src_file) {
  BufferedReader reader(src_file);
  BufferedWriter writer;

  std::string previous_key = {};
  std::string current_line;

  while (reader.ReadLine(&current_line)) {
    std::istringstream stream(current_line);

    std::string current_key;
    std::getline(stream, current_key, kKeyValueDelimiter);

    if (current_key != previous_key) {
      writer.Close();
      CreateTemporaryFile();
      writer.Open(temp_files_.back());
      previous_key = current_key;
    }

    std::string current_value;
    std::getline(stream, current_value, kLinesDelimiter);

    writer << current_key << kKeyValueDelimiter << current_value
           << kLinesDelimiter;
  }
}

int TempFilesManager::RunScriptOnTemporaryFiles(
    const std::string& script_path) {
  int number_of_processes = temp_files_.size();
  int completed_processes = 0;
  int processes_per_cycle =
      kNumberOfProcessesMultiplier * std::thread::hardware_concurrency();

  std::vector<bp::child> processes;
  processes.reserve(number_of_processes);

  while (completed_processes < number_of_processes) {
    int processes_to_run = std::min(
        processes_per_cycle,
        number_of_processes - completed_processes);

    for (int i = completed_processes;
         i < completed_processes + processes_to_run; ++i) {
      processes.emplace_back(script_path, temp_files_[i], temp_files_[i]);
    }

    int return_code = 0;
    for (auto& process : processes) {
      process.wait();
      return_code += process.exit_code();
    }

    if (return_code != 0) {
      return 1;
    }

    processes.clear();
    completed_processes += processes_to_run;
  }

  return 0;
}

void TempFilesManager::SortLinesInTemporaryFiles() {
  ThreadPool threadpool(
      kNumberOfThreadsMultiplier * std::thread::hardware_concurrency());
  for (const auto& file : temp_files_) {
    threadpool.Schedule([this, &file] { SortTemporaryFile(file); });
  }
  threadpool.StartWorkers();
}

void TempFilesManager::MergeTemporaryFiles(
    const std::string& dst_file) const {
  std::ofstream out(dst_file, std::ios::binary);
  for (const auto& file : temp_files_) {
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
    std::string line;
    int file_number;

    bool operator<(const HeapEntry& other) const {
      return line > other.line;
    }
  };

  std::vector<HeapEntry> heap(number_of_files);
  for (int i = 0; i < number_of_files; ++i) {
    readers[i].ReadLine(&heap[i].line);
    heap[i].file_number = i;
  }
  std::make_heap(heap.begin(), heap.end());

  while (!heap.empty()) {
    writer << heap.front().line << kLinesDelimiter;
    std::pop_heap(heap.begin(), heap.end());
    if (readers[heap.back().file_number].ReadLine(&heap.back().line)) {
      std::push_heap(heap.begin(), heap.end());
    } else {
      heap.pop_back();
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
  std::vector<std::string> lines;
  lines.reserve(kNumberOfLinesInBlock);

  BufferedReader reader(file);
  std::string current_line;
  while (reader.ReadLine(&current_line)) {
    lines.push_back(std::move(current_line));
  }
  reader.Close();

  std::sort(lines.begin(), lines.end());

  BufferedWriter writer(file);
  for (const auto& line : lines) {
    writer << line << kLinesDelimiter;
  }
}
