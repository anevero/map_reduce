#include <iostream>
#include <string>

#include "absl/status/status.h"
#include <boost/filesystem.hpp>

#include "utils/constants.h"
#include "mapreduce_lib/temp_files_manager.h"
#include "utils/utils.h"

namespace bf = boost::filesystem;

absl::Status RunMapScript(const std::string& script,
                          const std::string& src_file,
                          const std::string& dst_file) {
  TempFilesManager temp_files_manager;
  temp_files_manager.CreateTemporaryFilesByBlockSize(src_file);
  auto status = temp_files_manager.RunScriptOnTemporaryFiles(script);
  temp_files_manager.MergeTemporaryFiles(dst_file);
  return status;
}

void RunSort(const std::string& src_file,
             const std::string& dst_file) {
  TempFilesManager temp_files_manager;
  temp_files_manager.CreateTemporaryFilesByBlockSize(src_file);
  temp_files_manager.SortLinesInTemporaryFiles();
  temp_files_manager.MergeSortedTemporaryFiles(dst_file);
}

absl::Status RunReduceScript(const std::string& script,
                             const std::string& src_file,
                             const std::string& dst_file) {
  TempFilesManager temp_files_manager;
  temp_files_manager.CreateTemporaryFilesByKeys(src_file);
  auto status = temp_files_manager.RunScriptOnTemporaryFiles(script);
  temp_files_manager.MergeTemporaryFiles(dst_file);
  return status;
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Not enough arguments for this program.\n";
    return 1;
  }

  std::string str_operation = argv[1];
  std::string script = argv[2];
  std::string src_file = argv[3];
  std::string dst_file = argv[4];

  if (str_operation != constants::kMapOperationString
      && str_operation != constants::kReduceOperationString) {
    std::cerr << "Undefined operation '" << str_operation << "'." << std::endl;
    return 1;
  }

  if (auto status = utils::ValidateFile(script); !status.ok()) {
    std::cerr << status << std::endl;
    return 1;
  }

  if (auto status = utils::ValidateFile(src_file); !status.ok()) {
    std::cerr << status << std::endl;
    return 1;
  }

  auto status = absl::OkStatus();
  if (str_operation == constants::kMapOperationString) {
    status = RunMapScript(script, src_file, dst_file);
  } else {
    std::string temp_sorted_file = bf::unique_path().native();
    RunSort(src_file, temp_sorted_file);
    status = RunReduceScript(script, temp_sorted_file, dst_file);
    bf::remove(temp_sorted_file);
  }

  if (!status.ok()) {
    std::cerr << status << std::endl;
    return 1;
  }

  return 0;
}
