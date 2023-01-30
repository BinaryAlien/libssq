# libssq

☢️ An up-to-date [Source Server query protocol](https://developer.valvesoftware.com/wiki/Server_queries) library written in C99.

## Features

This library supports each of the three undeprecated A2S queries:
* A2S\_INFO,
* A2S\_PLAYER,
* and A2S\_RULES.

It has **no dependencies** and is designed to cross-compile on both **Windows** and **UNIX-like** operating systems.

However, it does **not** currently support **Goldsource** nor **compressed** responses.

## Documentation

The [wiki of this repository](https://github.com/BinaryAlien/libssq/wiki) provides documentation about the library and gets updated accordingly.

To quickly get started, check out the [example program](https://github.com/BinaryAlien/libssq/blob/main/example/example.c).

## Build

This project comes with a `CMakeLists.txt` to use with [CMake](https://cmake.org/) in order to generate a build system to compile the library on your machine.

### Example

* Compiling the library from the root of the repository using CMake.
```sh
$ pwd
~/libssq
$ mkdir build
$ cd build
$ cmake ..        # Generate the build system
$ cmake --build . # Build the library
```

* Compiling the [example program](https://github.com/BinaryAlien/libssq/blob/main/example/example.c) using [GCC](https://gcc.gnu.org/) from the root of the repository.
```sh
$ gcc -std=c99 -Iinclude -o ssq example/example.c -Lbuild -lssq
$ ./ssq
usage: ./ssq hostname [port]
```
