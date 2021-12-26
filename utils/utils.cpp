#include "utils/utils.h"

#include <iostream>

#include "boost/filesystem.hpp"

namespace utils {

namespace bf = boost::filesystem;

absl::Status ValidateFile(const std::string& path) {
  if (!bf::exists(path) || !bf::is_regular_file(path)) {
    return absl::InternalError(
        "File '" + path + "' doesn't exist or isn't a regular file");
  }
  return absl::OkStatus();
}

}  // namespace utils
