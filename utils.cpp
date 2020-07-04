#include "boost/filesystem.hpp"
#include "utils.h"

#include <iostream>

namespace utils {

namespace bf = boost::filesystem;

UserId ParseUserIdFromString(const std::string& string) {
  UserId id;
  try {
    id = std::stoull(string);
  } catch (...) {
    std::cerr << "Invalid user id: " << string << std::endl;
    id = -1;
  }
  return id;
}

SiteAddress ParseSiteAddressFromString(std::string string) {
  return string;
}

Time ParseTimeFromString(const std::string& string) {
  Time time;
  try {
    time = std::stoull(string);
  } catch (...) {
    std::cerr << "Invalid time: " << string << std::endl;
    time = -1;
  }
  return time;
}

int ValidateFile(const std::string& path) {
  if (!bf::exists(path) || !bf::is_regular_file(path)) {
    std::cerr << "File '" << path
              << "' doesn't exist or isn't a regular file.\n";
    return 1;
  }

  return 0;
}

}  // namespace utils
