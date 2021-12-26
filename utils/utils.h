#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>
#include <string>

#include "absl/status/status.h"

namespace utils {

absl::Status ValidateFile(const std::string& path);

}  // namespace utils

#endif  // UTILS_H_
