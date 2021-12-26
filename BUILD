load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

proto_library(
    name = "data_piece_proto",
    srcs = ["data_piece.proto"],
)

cc_proto_library(
    name = "data_piece_cc_proto",
    deps = [":data_piece_proto"],
)

cc_library(
    name = "utils",
    srcs = ["utils/utils.cpp"],
    hdrs = [
        "utils/constants.h",
        "utils/utils.h",
    ],
    deps = [
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
        "@boost//:filesystem",
    ],
)

cc_library(
    name = "buffered_io",
    srcs = [
        "buffered_io/buffered_reader.cpp",
        "buffered_io/buffered_writer.cpp",
    ],
    hdrs = [
        "buffered_io/buffered_reader.h",
        "buffered_io/buffered_writer.h",
    ],
    deps = [
        ":data_piece_cc_proto",
        ":utils",
        "@absl//absl/status:statusor",
    ],
)

cc_library(
    name = "mapreduce_lib",
    srcs = [
        "mapreduce_lib/process_manager.cpp",
        "mapreduce_lib/temp_files_manager.cpp",
        "mapreduce_lib/threadpool.cpp",
    ],
    hdrs = [
        "mapreduce_lib/process_manager.h",
        "mapreduce_lib/temp_files_manager.h",
        "mapreduce_lib/threadpool.h",
    ],
    linkopts = ["-lpthread"],
    deps = [
        ":buffered_io",
        ":data_piece_cc_proto",
        ":utils",
        "@absl//absl/status",
        "@boost//:filesystem",
        "@boost//:process",
    ],
)

cc_binary(
    name = "mapreduce",
    srcs = ["mapreduce.cpp"],
    deps = [
        ":mapreduce_lib",
        "@absl//absl/status",
    ],
)

cc_binary(
    name = "map",
    srcs = ["examples/map.cpp"],
    deps = [
        ":buffered_io",
        ":utils",
        "@boost//:filesystem",
    ],
)

cc_binary(
    name = "reduce",
    srcs = ["examples/reduce.cpp"],
    deps = [
        ":buffered_io",
        ":utils",
        "@boost//:filesystem",
    ],
)

cc_binary(
    name = "test_map_reduce",
    srcs = ["examples/test_map_reduce.cpp"],
    deps = [
        ":buffered_io",
        ":data_piece_cc_proto",
        "@absl//absl/random",
        "@boost//:filesystem",
        "@boost//:process",
    ],
)
