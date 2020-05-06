#include <iostream>
#include <string>

#include "mapreduce.h"

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Not enough arguments for this program.\n";
    return 1;
  }

  std::string str_operation = argv[1];
  std::string script = argv[2];
  std::string src_file = argv[3];
  std::string dst_file = argv[4];

  if (str_operation != "map" && str_operation != "reduce") {
    std::cerr << "Undefined operation '" << str_operation << "'.\n";
    return 1;
  }

  auto operation = (str_operation == "map") ? MapReduce::Operation::MAP
                                            : MapReduce::Operation::REDUCE;

  auto map_reduce = MapReduce(operation, script, src_file, dst_file);

  int arguments_checks_return_code = map_reduce.ValidateFiles();
  if (arguments_checks_return_code != 0) {
    return arguments_checks_return_code;
  }

  map_reduce.RunScript();

  return 0;
}
