#ifndef TEMP_FILES_MANAGER_H_
#define TEMP_FILES_MANAGER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "absl/status/status.h"

class FileOperationsManager {
 public:
  FileOperationsManager() = default;
  ~FileOperationsManager();

  // Returns filename (of the temporary file) by its ID.
  std::string GetFilenameById(int file_id) const;

  // Divides the file into several temporary files of the similar size and
  // returns an array of IDs of these temporary files.
  std::vector<int> DivideFileByBlockSize(const std::string& src_file);

  // Divides the file into several temporary files, containing entries with
  // the same key, and returns an array of IDs of these temporary files.
  // Assumes that the entries in the source file are sorted by the key.
  std::vector<int> DivideFileByKey(const std::string& src_file);

  // Merges the passed files into the destination file and removes these files.
  void MergeTemporaryFiles(const std::vector<int>& files_ids,
                           const std::string& dst_file);

  // Sorts each of the passed files by the key and the subkey. Multithreading
  // is used here.
  void SortLinesInTemporaryFiles(const std::vector<int>& files_ids) const;

  // Merges the passed files into the destination file and removes these files.
  // Ensures the destination file contains all the entries in the sorted order.
  // Assumes all the entries in the passed files are already sorted.
  void MergeSortedTemporaryFiles(const std::vector<int>& files_ids,
                                 const std::string& dst_file);

  // Creates a necessary amount of temporary files and returns an array of
  // their IDs.
  std::vector<int> CreateTemporaryFiles(int count);

  // Removes all the temporary files with IDs specified in the passed array.
  void RemoveTemporaryFiles(const std::vector<int>& files_ids);

  // Removes all the temporary files created. All the IDs become invalid after
  // this operation.
  void RemoveAllTemporaryFiles();

 private:
  // Creates a temporary file with a random name and returns its ID.
  int CreateTemporaryFile();

  // Removes a file by ID.
  void RemoveTemporaryFile(int file_id);

  // Sorts the file by the ID.
  void SortTemporaryFile(int file_id) const;

 private:
  int current_file_id_ = 0;
  std::unordered_map<int, std::string> temp_files_;
};

#endif  // TEMP_FILES_MANAGER_H_
