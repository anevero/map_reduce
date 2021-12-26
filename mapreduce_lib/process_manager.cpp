#include "mapreduce_lib/process_manager.h"
#include "utils/constants.h"

#include <thread>

#include <boost/process.hpp>

namespace bp = boost::process;

ProcessManager::ProcessManager(const std::vector<std::string>& temp_files)
    : temp_files_(temp_files) {}

absl::Status ProcessManager::RunAndWait(const std::string& script_path) {
  int processes_number = temp_files_.size();
  int completed_processes = 0;
  int processes_per_cycle = constants::kNumberOfProcessesMultiplier *
      std::thread::hardware_concurrency();

  if (processes_per_cycle == 0) {
    processes_per_cycle = constants::kNumberOfProcessesMultiplier;
  }

  std::vector<bp::child> current_processes;
  current_processes.reserve(processes_number);

  while (completed_processes < processes_number) {
    int processes_to_run = std::min(
        processes_per_cycle,
        processes_number - completed_processes);

    for (int i = completed_processes;
         i < completed_processes + processes_to_run; ++i) {
      current_processes.emplace_back(
          script_path, temp_files_[i], temp_files_[i]);
    }

    int return_code = 0;
    for (auto& process: current_processes) {
      process.wait();
      return_code += process.exit_code();
    }

    if (return_code != 0) {
      return absl::InternalError("Some processes exited with non-zero code");
    }

    current_processes.clear();
    completed_processes += processes_to_run;
  }

  return absl::OkStatus();
}
