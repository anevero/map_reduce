#include <iostream>
#include <string>

#include "constants.h"
#include "temp_files_manager.h"
#include "utils.h"

int RunMapScript(const std::string& script,
                 const std::string& src_file,
                 const std::string& dst_file) {
  TempFilesManager temp_files_manager;
  temp_files_manager.CreateTemporaryFilesByBlockSize(src_file);
  int return_code = temp_files_manager.RunScriptOnTemporaryFiles(script);
  temp_files_manager.MergeTemporaryFiles(dst_file);

  return return_code;
}

void RunSort(const std::string& src_file,
             const std::string& dst_file) {
  TempFilesManager temp_files_manager;
  temp_files_manager.CreateTemporaryFilesByBlockSize(src_file);
  temp_files_manager.SortLinesInTemporaryFiles();
  temp_files_manager.MergeSortedTemporaryFiles(dst_file);
}

int RunReduceScript(const std::string& script,
                    const std::string& src_file,
                    const std::string& dst_file) {
  TempFilesManager temp_files_manager;
  temp_files_manager.CreateTemporaryFilesByKeys(src_file);
  int return_code = temp_files_manager.RunScriptOnTemporaryFiles(script);
  temp_files_manager.MergeTemporaryFiles(dst_file);

  return return_code;
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

  if (utils::ValidateFile(script) != 0) {
    std::cerr << "Script file '" << script << "' is incorrect." << std::endl;
    return 1;
  }

  if (utils::ValidateFile(src_file) != 0) {
    std::cerr << "Source file '" << src_file << "' is incorrect." << std::endl;
    return 1;
  }

  if (str_operation == constants::kMapOperationString) {
    return RunMapScript(script, src_file, dst_file);
  }

  std::string temp_sorted_file = bf::unique_path().native();
  RunSort(src_file, temp_sorted_file);
  std::clog << "Sorting complete!" << std::endl;
  int return_code = RunReduceScript(script, temp_sorted_file, dst_file);
  bf::remove(temp_sorted_file);

  return return_code;
}
