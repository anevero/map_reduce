#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include <string>
#include <vector>

class ProcessManager {
 public:
  explicit ProcessManager(const std::vector<std::string>& temp_files);
  ~ProcessManager() = default;

  void RunAndWait(const std::string& script_path);

 private:
  const std::vector<std::string>& temp_files_;
};

#endif  // PROCESS_MANAGER_H_
