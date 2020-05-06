#ifndef MAPREDUCE_H_
#define MAPREDUCE_H_

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include "constants.h"

namespace bp = boost::process;
namespace bf = boost::filesystem;

class MapReduce {
 public:
  enum class Operation {
    MAP,
    REDUCE
  };

  /// MapReduce constructor. <br>
  /// Passed files paths should be correct. They can be additionally checked
  /// later with help of the corresponding function.
  /// \param operation Map or Reduce operation.
  /// \param script Path to the script file.
  /// \param src_file Path to the source file.
  /// \param dst_file Path to the destination file.
  MapReduce(Operation operation,
            std::string script,
            std::string src_file,
            std::string dst_file);

  /// MapReduce destructor. <br>
  /// Assures that all the temporary files are removed.
  ~MapReduce();

  /// Checks if internal script, src_file and dst_file paths are real
  /// paths of some real files.
  /// \retval 1 Script or source file files are empty or do not exist.
  /// \retval 0 Everything seems OK right now, but nevertheless some problems
  /// can appear later.
  int ValidateFiles() const;

  void SetOperation(Operation operation);
  void SetScript(const std::string& script);
  void SetSourceFile(const std::string& src_file);
  void SetDestinationFile(const std::string& dst_file);

  /// Runs the internal script (passed to the constructor or set with the
  /// corresponding function). <br>
  /// Some additional operations can be executed before running the script
  /// (depending on the current operation). <br>
  /// Some additional temporary files can be created while running the script.
  /// If everything will be OK, they will be removed automatically (if not -
  /// in the destructor of the class).
  /// \returns Script return value. If the script is executed several times,
  /// this function stops executing and returns as soon as the script returns
  /// non-null value.
  int RunScript();

 private:
  struct ReduceTempFiles {
    std::vector<std::string> input_files;
    std::vector<std::string> output_files;
  };

  /// Checks if the passed file is a real file.
  /// \retval 1 The file is empty or does not exist.
  /// \retval 0 Everything seems OK right now, but nevertheless some problems
  /// can appear later.
  int ValidateFile(const std::string& path) const;

  /// Removes all the temporary files, which names are stored in the internal
  /// temp_files_ vector.
  void RemoveTemporaryFiles();

  /// Runs Map script, which uses passed strings as source and destination
  /// files paths.
  /// \returns Script return value.
  /// \remark Map script uses stdin as the source and stdout as the
  /// destination. Input and output are redirected manually.
  int RunMapScript(const std::string& src_file,
                   const std::string& dst_file) const;

  /// Runs Reduce script, which uses the passed file as the source and
  /// destination file.
  /// Passed file must be created by the ShuffleAndSort function before.
  /// \returns Script return value.
  /// \remark Reduce script uses stdin as the source and stdout as the
  /// destination. Input and output are redirected manually.
  int RunReduceScript(const ReduceTempFiles& files) const;

  /// Loads all the data from the source file to the memory,
  /// divides it on clusters according to the keys, writes these
  /// clusters to the created temporary files.
  /// \returns Struct of two vectors (of created temporary files names).
  ReduceTempFiles SplitPairs(const std::string& src_file);

  /// A helper function for the OrderPairs function. Creates the necessary
  /// number of temporary files.
  /// \returns Struct of two vectors (of created temporary files names).
  ReduceTempFiles CreateTempFiles(int number_of_files);

  /// Merges the pairs from all the passed files into one file.
  void MergePairs(const std::vector<std::string>& files,
                  const std::string& dst_file) const;

  /// Sorts the pairs in the source file and writes the output to the
  /// destination file.
  void SortPairs(const std::string& src_file,
                 const std::string& dst_file) const;

 private:
  Operation operation_;
  std::string script_;
  std::string src_file_;
  std::string dst_file_;

  std::vector<std::string> temp_files_;
};

#endif  // MAPREDUCE_H_
