#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include <string>
#include <vector>

#include "absl/status/status.h"

class ProcessManager {
 public:
  explicit ProcessManager(const std::vector<std::string>& temp_files);
  ~ProcessManager() = default;

  absl::Status RunAndWait(const std::string& script_path);

 private:
  const std::vector<std::string>& temp_files_;
};

#endif  // PROCESS_MANAGER_H_
