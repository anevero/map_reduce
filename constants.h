#ifndef CONSTANTS_H_
#define CONSTANTS_H_

namespace constants {

const char* const kMapOperationString = "map";
const char* const kReduceOperationString = "reduce";

const char kKeyValueDelimiter = '\t';
const char kLinesDelimiter = '\n';
const char kWordsDelimiter = ' ';

const int kNumberOfLinesInBlock = 50000;
const int kReadBufferNumberOfLines = 7500;
const int kWriteBufferLength = 75000;

const int kNumberOfProcessesMultiplier = 3;
const int kNumberOfThreadsMultiplier = 3;

}  // namespace constants

#endif  // CONSTANTS_H_
