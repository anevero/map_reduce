#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#include "utils/constants.h"
#include "mapreduce_lib/file_operations_manager.h"
#include "mapreduce_lib/process_manager.h"
#include "utils/utils.h"

namespace bf = boost::filesystem;

void RunMapScript(const std::string& script,
                  const std::string& src_file,
                  const std::string& dst_file) {
  FileOperationsManager files_manager;
  auto input_files = files_manager.DivideFileByBlockSize(src_file);
  auto output_files = files_manager.CreateTemporaryFiles(input_files.size());
  ProcessManager::RunAndWait(script, input_files, output_files, files_manager);
  files_manager.RemoveTemporaryFiles(input_files);
  files_manager.MergeTemporaryFiles(output_files, dst_file);
}

void RunSort(const std::string& src_file,
             const std::string& dst_file) {
  FileOperationsManager files_manager;
  auto input_files = files_manager.DivideFileByBlockSize(src_file);
  files_manager.SortLinesInTemporaryFiles(input_files);
  files_manager.MergeSortedTemporaryFiles(input_files, dst_file);
}

void RunReduceScript(const std::string& script,
                     const std::string& src_file,
                     const std::string& dst_file) {
  FileOperationsManager files_manager;
  auto input_files = files_manager.DivideFileByKey(src_file);
  auto output_files = files_manager.CreateTemporaryFiles(input_files.size());
  ProcessManager::RunAndWait(script, input_files, output_files, files_manager);
  files_manager.RemoveTemporaryFiles(input_files);
  files_manager.MergeTemporaryFiles(output_files, dst_file);
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Incorrect arguments" << std::endl;
    std::cerr
        << "Example: ./mapreduce [map|reduce] ./script ./src_file ./dst_file"
        << std::endl;
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

  if (str_operation == constants::kMapOperationString) {
    RunMapScript(script, src_file, dst_file);
  } else {
    std::string temp_sorted_file = bf::unique_path().native();
    RunSort(src_file, temp_sorted_file);
    RunReduceScript(script, temp_sorted_file, dst_file);
    bf::remove(temp_sorted_file);
  }

  return 0;
}
