// This header and corresponding cpp file contains declarations and definitions
// of some project-wide functions. It helps to divide the logic of the project
// and allows to easily change, for example, some parsing logic in the future.

#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>
#include <string>

namespace utils {

using UserId = uint64_t;
using SiteAddress = std::string;
using Time = uint64_t;

UserId ParseUserIdFromString(const std::string& string);
SiteAddress ParseSiteAddressFromString(std::string string);
Time ParseTimeFromString(const std::string& string);

int ValidateFile(const std::string& path);

}  // namespace utils

#endif  // UTILS_H_
