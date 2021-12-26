load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

cc_library(
    name = "utils",
    srcs = ["utils.cpp"],
    hdrs = [
        "constants.h",
        "utils.h",
    ],
)

cc_library(
    name = "buffered_io",
    srcs = [
        "buffered_reader.cpp",
        "buffered_writer.cpp",
    ],
    hdrs = [
        "buffered_reader.h",
        "buffered_writer.h",
    ],
    deps = [":utils"],
)

cc_library(
    name = "mapreduce_deps",
    srcs = [
        "process_manager.cpp",
        "temp_files_manager.cpp",
        "threadpool.cpp",
    ],
    hdrs = [
        "process_manager.h",
        "temp_files_manager.h",
        "threadpool.h",
    ],
    linkopts = ["-lpthread"],
    deps = [
        ":buffered_io",
        ":utils",
        "@boost//:filesystem",
        "@boost//:process",
    ],
)

cc_binary(
    name = "mapreduce",
    srcs = ["mapreduce.cpp"],
    deps = [
        ":mapreduce_deps",
    ],
)

cc_binary(
    name = "map",
    srcs = ["map.cpp"],
    deps = [
        ":buffered_io",
        ":utils",
        "@boost//:filesystem",
    ],
)

cc_binary(
    name = "reduce",
    srcs = ["reduce.cpp"],
    deps = [
        ":buffered_io",
        ":utils",
        "@boost//:filesystem",
    ],
)
