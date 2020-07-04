// This script is used to convert the data from one format to another.

// Input format (non-binary):
// site_string \t user_id \t time \n
// ...
// site_string \t user_id \t time \n

// Output format (non-binary):
// user_id \t site \t time
// ...
// user_id \t site \t time

// The first argument to the script should be the name of the input file, the
// second - the name of the output file.

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

#include "buffered_reader.h"
#include "buffered_writer.h"
#include "constants.h"
#include "utils.h"

using utils::UserId;
using utils::SiteAddress;
using utils::Time;

struct SiteVisitEntry {
  SiteAddress site_address;
  Time time;

  SiteVisitEntry() : site_address(), time(0) {}

  SiteVisitEntry(SiteAddress site_address, Time time)
      : site_address(std::move(site_address)), time(time) {}
};

std::unordered_multimap<UserId, SiteVisitEntry> ReadEntries(
    const std::string& src_file) {
  std::unordered_multimap<UserId, SiteVisitEntry> entries;

  BufferedReader reader(src_file);
  std::string current_line;

  while (reader.ReadLine(&current_line)) {
    UserId user_id;
    SiteAddress site_address;
    Time time;

    std::istringstream line_stream(current_line);
    std::string current_string;

    std::getline(line_stream, current_string,
                 constants::kKeyValueDelimiter);
    site_address = utils::ParseSiteAddressFromString(
        std::move(current_string));

    std::getline(line_stream, current_string,
                 constants::kKeyValueDelimiter);
    user_id = utils::ParseUserIdFromString(current_string);

    std::getline(line_stream, current_string,
                 constants::kKeyValueDelimiter);
    time = utils::ParseTimeFromString(current_string);

    entries.emplace(user_id, SiteVisitEntry{site_address, time});
  }

  return entries;
}

void WriteEntries(
    const std::unordered_multimap<UserId, SiteVisitEntry>& entries,
    const std::string& dst_file) {
  BufferedWriter writer(dst_file);
  for (auto&&[user_id, entry] : entries) {
    writer << user_id << constants::kKeyValueDelimiter
           << entry.site_address << constants::kKeyValueDelimiter
           << entry.time << constants::kLinesDelimiter;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Not enough arguments for this program." << std::endl;
    return 1;
  }

  if (utils::ValidateFile(argv[1]) != 0) {
    std::cerr << "Input file path is incorrect." << std::endl;
    return 1;
  }

  WriteEntries(ReadEntries(argv[1]), argv[2]);

  return 0;
}
