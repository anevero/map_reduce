# MapReduce

This repository contains a simple implementation of the MapReduce model for the
distributed computations. In this example distribution is achieved by spawning
multiple processes using the Boost library.

Although the applied libraries are generally cross-platform, this code was
tested only on Linux systems. There is no guarantee that it will work properly
on Windows.

# Build instructions

This project can be built with the [Bazel](https://bazel.build/) build system.
It is responsible for downloading and building all the necessary components,
like Boost.Process and Boost.Filesystem modules, Google Protocol Buffers and
Abseil libraries. You will also need a Protocol Buffers compiler.

The project was developed in [CLion](https://www.jetbrains.com/clion/) with the
[Bazel plugin by Google](https://plugins.jetbrains.com/plugin/9554-bazel),
although this is not required.

# Run instructions

The project is divided into three parts:
- **MapReduce core**. It is responsible for running given map and reduce
scripts, handling errors, creating temporary files for them, and sorting the
data. It can work with any map and reduce scripts, which accept and produce the
data in a special format.
- **Map and Reduce scripts examples**. They solve a task of aggregating users'
browser histories. They take information about single visits and provide
statistics (overall visit time) for each user and site.
- **Testing utility**. It generates test data for the given Map and Reduce
scripts, runs them on it and checks if the results are correct.

To run Map or Reduce operation, you should use the MapReduce core executable.
The format is as follows:

`./mapreduce map <map_script> <src_file> <dst_file>`

`./mapreduce reduce <reduce_script> <src_file> <dst_file>`

To run the test utility, you can just execute the corresponding file without
any additional arguments.

Your own Map and Reduce scripts should accept and produce files, containing
[delimited](https://github.com/protocolbuffers/protobuf/blob/master/src/google/protobuf/util/delimited_message_util.h)
serialized representations of the DataPiece proto. Sorting operation performed
by the MapReduce core will work with the keys and the subkeys. Any reducer is
guaranteed to receive data belonging to the same key (but the subkeys can still
be different).

# Core classes short description

There are several utility classes that together form the core of the project.

- **Buffered I/O utilities**: since all the data transferring operations here
are implemented using files, efficient I/O algorithms are necessary. Google
Protocol Buffers allow storing data in the binary format (in order to consume
less space). Implemented I/O utilities allow reading and write data using
buffers, which makes these operations times faster.
- **File operations manager**: this class contains some higher level logic,
including the logic of creating and managing the temporary files, merging
files and external sort implementation (using the heap-sort algorithm).
- **Processes manager**: it's responsible for creating processes for Mappers
and Reducers with the necessary arguments, and also rerunning them in case of
failures.
- **Thread pool**: this routine is necessary for efficient implementation of
different multithreading operations. It's used, for example, inside the
external sort algorithm (to sort several blocks simultaneously).
