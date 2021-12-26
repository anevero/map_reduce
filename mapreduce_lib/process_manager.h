#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include <string>
#include <vector>

#include "mapreduce_lib/file_operations_manager.h"

class ProcessManager {
 public:
  static void RunAndWait(const std::string& script_path,
                         const std::vector<int>& input_files_ids,
                         const std::vector<int>& output_files_ids,
                         const FileOperationsManager& file_operations_manager);
};

#endif  // PROCESS_MANAGER_H_
