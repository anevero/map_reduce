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
  return (ValidateFile(script_) == 1 || ValidateFile(src_file_) == 1) ? 1 : 0;
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

  auto files = CombinePairs();
  int return_code = RunReduceScript(files);

  if (return_code != 0) {
    RemoveTemporaryFiles();
    return return_code;
  }

  // Creating a temporary file for unsorted pairs data.
  temp_files_.push_back(bf::unique_path().native());
  MergePairs(files.output_files, temp_files_.back());
  SortPairs(temp_files_.back(), dst_file_);

  RemoveTemporaryFiles();
  return return_code;
}

int MapReduce::ValidateFile(const std::string& path) const {
  auto file_path = std::filesystem::path(path);

  if (!std::filesystem::exists(file_path) ||
      !std::filesystem::is_regular_file(file_path)) {
    std::cerr << "File '" << path
              << "' doesn't exist or isn't a regular file.\n";
    return 1;
  }

  return 0;
}

void MapReduce::RemoveTemporaryFiles() {
  while (!temp_files_.empty()) {
    auto temp_file_path = std::filesystem::path(temp_files_.back());
    std::filesystem::remove(temp_file_path);
    temp_files_.pop_back();
  }
}

int MapReduce::RunMapScript(const std::string& src_file,
                            const std::string& dst_file) const {
  return bp::system(script_, bp::std_in < src_file, bp::std_out > dst_file);
}

int MapReduce::RunReduceScript(const ReduceTempFiles& files) const {
  int number_of_files = files.input_files.size();
  std::vector<bp::child> processes;
  processes.reserve(number_of_files);

  for (int i = 0; i < number_of_files; ++i) {
    processes.emplace_back(script_,
                           bp::std_in < files.input_files[i],
                           bp::std_out > files.output_files[i]);
  }

  int return_code = 0;
  for (int i = 0; i < number_of_files; ++i) {
    processes[i].wait();
    return_code += processes[i].exit_code();
  }

  return (return_code == 0) ? 0 : 1;
}

MapReduce::ReduceTempFiles MapReduce::CombinePairs() {
  std::unordered_map<std::string, std::vector<std::string>> pairs;
  std::ifstream in(src_file_);

  std::string current_line;
  while (std::getline(in, current_line, kLinesDelimiter)) {
    std::istringstream line_stream(current_line);
    std::string key;
    std::string value;  // always equal to "1" in our task
    std::getline(line_stream, key, kKeyValueDelimiter);
    std::getline(line_stream, value, kLinesDelimiter);
    pairs[std::move(key)].push_back(std::move(value));
  }
  in.close();

  ReduceTempFiles files = CreateTempFiles(pairs.size());

  int i = 0;
  for (auto&&[key, values] : pairs) {
    std::ofstream out(files.input_files[i]);
    for (const auto& value : values) {
      out << key << kKeyValueDelimiter << value << kLinesDelimiter;
    }
    ++i;
  }

  return files;
}

MapReduce::ReduceTempFiles MapReduce::CreateTempFiles(int number_of_files) {
  std::vector<std::string> input_files;
  std::vector<std::string> output_files;
  input_files.reserve(number_of_files);
  output_files.reserve(number_of_files);
  temp_files_.reserve(temp_files_.size() + 2 * number_of_files + 1);

  for (int i = 0; i < number_of_files; ++i) {
    input_files.push_back(bf::unique_path().native());
    output_files.push_back(bf::unique_path().native());

    temp_files_.push_back(input_files.back());
    temp_files_.push_back(output_files.back());
  }

  return {input_files, output_files};
}

void MapReduce::MergePairs(const std::vector<std::string>& files,
                           const std::string& dst_file) const {
  std::ofstream out(dst_file);
  for (const auto& file : files) {
    std::ifstream in(file);
    std::string current_line;
    while (std::getline(in, current_line, kLinesDelimiter)) {
      out << current_line << kLinesDelimiter;
    }
  }
}

// External sorting will be later implemented / called here.
void MapReduce::SortPairs(const std::string& src_file,
                          const std::string& dst_file) const {
  std::vector<std::pair<std::string, std::string>> pairs;

  std::ifstream in(src_file);
  std::string current_line;

  while (std::getline(in, current_line, kLinesDelimiter)) {
    std::string key;
    std::string value;

    std::istringstream line_stream(current_line);
    std::getline(line_stream, key, kKeyValueDelimiter);
    std::getline(line_stream, value, kLinesDelimiter);

    pairs.emplace_back(std::move(key), std::move(value));
  }

  std::sort(pairs.begin(), pairs.end(), [](
      const std::pair<std::string, std::string>& a,
      const std::pair<std::string, std::string>& b) {
    return a.first < b.first;
  });

  std::ofstream out(dst_file);
  for (auto&&[key, value] : pairs) {
    out << key << kKeyValueDelimiter << value << kLinesDelimiter;
  }
}
