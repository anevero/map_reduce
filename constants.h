#ifndef MAPREDUCE_CONSTANTS_H_
#define MAPREDUCE_CONSTANTS_H_

const char kKeyValueDelimiter = '\t';
const char kLinesDelimiter = '\n';
const char kWordsDelimiter = ' ';

const int kNumberOfLinesInBlock = 50000;  // 50000
const int kReadBufferNumberOfLines = 7500;  // 7500
const int kWriteBufferLength = 75000;

#endif  // MAPREDUCE_CONSTANTS_H_
