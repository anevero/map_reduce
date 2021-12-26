#ifndef TEMP_FILES_MANAGER_H_
#define TEMP_FILES_MANAGER_H_

#include <string>
#include <vector>

#include "absl/status/status.h"

class TempFilesManager {
 public:
  TempFilesManager() = default;
  ~TempFilesManager();

  void CreateTemporaryFilesByBlockSize(const std::string& src_file);
  void CreateTemporaryFilesByKeys(const std::string& src_file);

  absl::Status RunScriptOnTemporaryFiles(const std::string& script_path);
  void SortLinesInTemporaryFiles();

  void MergeTemporaryFiles(const std::string& dst_file) const;
  void MergeSortedTemporaryFiles(const std::string& dst_file) const;
  void RemoveTemporaryFiles();

 private:
  void CreateTemporaryFile();
  void SortTemporaryFile(const std::string& file);

 private:
  std::vector<std::string> temp_files_;
};

#endif  // TEMP_FILES_MANAGER_H_
