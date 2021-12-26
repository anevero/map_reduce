#include "mapreduce_lib/process_manager.h"
#include "utils/constants.h"

#include <iostream>
#include <map>
#include <memory>
#include <thread>

#include <boost/process.hpp>

namespace bp = boost::process;

ProcessManager::ProcessManager(const std::vector<std::string>& temp_files)
    : temp_files_(temp_files) {}

void ProcessManager::RunAndWait(const std::string& script_path) {
  int processes_number = temp_files_.size();
  int processes_per_cycle = constants::kNumberOfProcessesMultiplier *
      std::thread::hardware_concurrency();

  if (processes_per_cycle == 0) {
    processes_per_cycle = constants::kNumberOfProcessesMultiplier;
  }

  std::map<int, std::unique_ptr<bp::child>> processes;
  int passed_processes = 0;
  int failed_processes = 0;

  while (passed_processes < processes_number || !processes.empty()) {
    int old_processes_to_rerun = processes.size();
    failed_processes += old_processes_to_rerun;

    int new_processes_to_run = std::min(
        processes_per_cycle,
        processes_number - passed_processes);
    new_processes_to_run -= old_processes_to_rerun;

    for (int i = passed_processes;
         i < passed_processes + new_processes_to_run; ++i) {
      processes.emplace(i, nullptr);
    }
    if (new_processes_to_run > 0) {
      passed_processes += new_processes_to_run;
    }

    for (auto&[index, process]: processes) {
      process = std::make_unique<bp::child>(
          script_path, temp_files_[index], temp_files_[index]);
    }

    auto it = processes.begin();
    while (it != processes.end()) {
      it->second->wait();
      if (it->second->exit_code() != 0) {
        it->second = nullptr;
        ++it;
      } else {
        it = processes.erase(it);
      }
    }
  }

  std::clog << failed_processes << " processes failed and were rerun"
            << std::endl;
}
