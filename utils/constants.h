#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "absl/strings/string_view.h"

namespace constants {

constexpr absl::string_view kMapOperationString = "map";
constexpr absl::string_view kReduceOperationString = "reduce";

constexpr int kNumberOfLinesInBlock = 50000;
constexpr int kReadBufferNumberOfLines = 7500;
constexpr int kWriteBufferLength = 75000;

constexpr int kNumberOfProcessesMultiplier = 3;
constexpr int kNumberOfThreadsMultiplier = 3;

}  // namespace constants

#endif  // CONSTANTS_H_
