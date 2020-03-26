#include "mapreduce.h"

MapReduce::MapReduce(Operation operation,
                     std::string script,
                     std::string src_file,
                     std::string dst_file)
    : operation_(operation),
      script_(std::move(script)),
      src_file_(std::move(src_file)),
      dst_file_(std::move(dst_file)) {
}

MapReduce::~MapReduce() {
  RemoveTemporaryFiles();
}

int MapReduce::ValidateFiles() const {
  auto script_file_path = std::filesystem::path(script_);
  auto src_file_path = std::filesystem::path(src_file_);
  auto dst_file_path = std::filesystem::path(dst_file_);

  if (!std::filesystem::exists(script_file_path) ||
      !std::filesystem::is_regular_file(script_file_path)) {
    std::cerr << "Script file '" << script_
              << "' doesn't exist or isn't a regular file.\n";
    return 1;
  }

  if (!std::filesystem::exists(src_file_path) ||
      !std::filesystem::is_regular_file(src_file_path)) {
    std::cerr << "Source file '" << src_file_
              << "' doesn't exist or isn't a regular file.\n";
    return 1;
  }

  if (std::filesystem::exists(dst_file_path)) {
    std::clog << "Destination file '" << dst_file_
              << "' already exists and will be overwritten.\n";
  }

  return 0;
}

void MapReduce::SetOperation(Operation operation) {
  operation_ = operation;
}

void MapReduce::SetScript(const std::string& script) {
  script_ = script;
}

void MapReduce::SetSourceFile(const std::string& src_file) {
  src_file_ = src_file;
}

void MapReduce::SetDestinationFile(const std::string& dst_file) {
  dst_file_ = dst_file;
}

int MapReduce::RunScript() {
  if (operation_ == Operation::MAP) {
    return RunMapScript(src_file_, dst_file_);
  }

  int return_code = 0;
  auto files = ShuffleAndSort();

  int number_of_files = files.input_files.size();
  for (int i = 0; i < number_of_files; ++i) {
    return_code = RunReduceScript(files.input_files[i], files.output_files[i]);
    if (return_code != 0) {
      break;
    }
  }

  if (return_code != 0) {
    RemoveTemporaryFiles();
    return return_code;
  }

  std::ofstream out(dst_file_, std::ios::binary);
  for (auto&& file : files.output_files) {
    std::ifstream in(file);
    std::string current_line;
    while (std::getline(in, current_line, LINES_DELIMITER)) {
      out << current_line << LINES_DELIMITER;
    }
  }

  RemoveTemporaryFiles();
  return return_code;
}

void MapReduce::RemoveTemporaryFiles() {
  while (!temp_files_.empty()) {
    auto temp_file_path = std::filesystem::path(temp_files_.back());
    std::filesystem::remove(temp_file_path);
    temp_files_.pop_back();
  }
}

int MapReduce::RunMapScript(const std::string& src_file,
                            const std::string& dst_file) {
  return bp::system(script_, bp::std_in < src_file, bp::std_out > dst_file);
}

int MapReduce::RunReduceScript(const std::string& src_file,
                               const std::string& dst_file) {
  return bp::system(script_, bp::std_in < src_file, bp::std_out > dst_file);
}

MapReduce::ReduceTempFiles MapReduce::ShuffleAndSort() {
  std::map<std::string, std::vector<int>> pairs;
  std::ifstream in(src_file_, std::ios::binary);

  std::string current_line;
  while (std::getline(in, current_line, LINES_DELIMITER)) {
    std::istringstream line_stream(current_line);
    std::string key, value;
    std::getline(line_stream, key, KEY_VALUE_DELIMITER);
    std::getline(line_stream, value, LINES_DELIMITER);
    pairs[key].push_back(std::stoi(value));
  }
  in.close();

  int number_of_keys = pairs.size();
  std::vector<std::string> input_files;
  std::vector<std::string> output_files;
  input_files.reserve(number_of_keys);
  output_files.reserve(number_of_keys);
  temp_files_.reserve(temp_files_.size() + 2 * number_of_keys);

  std::string time = std::to_string(
      std::chrono::duration(
          std::chrono::system_clock::now().time_since_epoch()).count());
  std::string temp_file_prefix = ".tmp." + time + ".";

  for (int i = 0; i < number_of_keys; ++i) {
    input_files.emplace_back(temp_file_prefix + "in." + std::to_string(i));
    output_files.emplace_back(temp_file_prefix + "out." + std::to_string(i));
    temp_files_.push_back(input_files.back());
    temp_files_.push_back(output_files.back());
  }

  int i = 0;
  for (auto&&[key, values] : pairs) {
    std::ofstream out(input_files[i], std::ios::binary);
    for (auto&& value : values) {
      out << key << KEY_VALUE_DELIMITER << value << LINES_DELIMITER;
    }
    ++i;
  }

  return {input_files, output_files};
}
