// This script is used to merge all the data from the one user.

// Input format (non-binary):
// user_id \t site_address \t time \n
// ...
// user_id \t site_address \t time \n

// Output format (non-binary):
// User #user_id \n
// site \t average_time \t min_time \t max_time \n
// ...
// site \t average_time \t min_time \t max_time \n

// The first argument to the script should be the name of the input file, the
// second - the name of the output file. All the entries in the input file must
// have the same user id!

#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "buffered_reader.h"
#include "buffered_writer.h"
#include "constants.h"
#include "utils.h"

using utils::UserId;
using utils::SiteAddress;
using utils::Time;

class SiteInfo {
 public:
  SiteInfo(const SiteAddress& site_address,
           const std::vector<Time>& visit_times)
      : site_address_(site_address) {
    for (Time visit_time : visit_times) {
      average_time_ += visit_time;  // assuming it won't overflow
      min_time_ = std::min(min_time_, visit_time);
      max_time_ = std::max(max_time_, visit_time);
    }
    average_time_ /= visit_times.size();
  }

  std::string ToString() const {
    std::ostringstream stream;
    stream << site_address_ << constants::kKeyValueDelimiter
           << average_time_ << constants::kKeyValueDelimiter
           << min_time_ << constants::kKeyValueDelimiter
           << max_time_;
    return stream.str();
  }

 private:
  std::string_view site_address_;
  Time average_time_ = 0;
  Time min_time_ = std::numeric_limits<Time>::max();
  Time max_time_ = std::numeric_limits<Time>::min();
};

auto ReadEntries(const std::string& src_file) {
  std::unordered_map<SiteAddress, std::vector<Time>> entries;

  BufferedReader reader(src_file);
  std::string current_line;
  UserId id = -1;

  while (reader.ReadLine(&current_line)) {
    UserId user_id;
    SiteAddress site_address;
    Time time;

    std::istringstream line_stream(current_line);
    std::string current_string;

    std::getline(line_stream, current_string,
                 constants::kKeyValueDelimiter);
    user_id = utils::ParseUserIdFromString(current_string);
    if (id == static_cast<UserId>(-1)) {
      id = user_id;
    } else if (user_id != id) {
      std::cerr << "Invalid format of the file." << std::endl;
      continue;
    }

    std::getline(line_stream, current_string,
                 constants::kKeyValueDelimiter);
    site_address = utils::ParseSiteAddressFromString(
        std::move(current_string));

    std::getline(line_stream, current_string,
                 constants::kKeyValueDelimiter);
    time = utils::ParseTimeFromString(current_string);

    entries[std::move(site_address)].push_back(time);
  }

  return std::tuple{id, entries};
}

std::vector<SiteInfo> ConvertEntries(
    const std::unordered_map<SiteAddress, std::vector<Time>>& entries) {
  std::vector<SiteInfo> result;
  result.reserve(entries.size());
  for (auto&&[site_address, visit_times] : entries) {
    result.emplace_back(site_address, visit_times);
  }
  return result;
}

void WriteEntries(UserId user_id, const std::vector<SiteInfo>& sites_info,
                  const std::string& dst_file) {
  BufferedWriter writer(dst_file);
  writer << "User #" << user_id << ":" << constants::kLinesDelimiter;
  for (const auto& site : sites_info) {
    writer << site.ToString() << constants::kLinesDelimiter;
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

  auto entries = ReadEntries(argv[1]);
  auto user_id = std::get<0>(entries);
  auto sites_info = ConvertEntries(std::get<1>(entries));

  WriteEntries(user_id, sites_info, argv[2]);

  return 0;
}
