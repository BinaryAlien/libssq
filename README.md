# libssq

☢️ An up-to-date [Source Server query protocol](https://developer.valvesoftware.com/wiki/Server_queries) library written in C99.

## Features

This library supports each of the three undeprecated A2S queries:
* A2S\_INFO,
* A2S\_PLAYER,
* and A2S\_RULES.

It has **no dependencies** and is designed to cross-compile on both **Windows** and **UNIX-like** operating systems.

It does **not** currently support **Goldsource** nor **compressed** responses.

## Documentation

The [wiki of this repository](https://github.com/BinaryAlien/libssq/wiki) provides documentation about the library and gets updated accordingly.

To quickly get started, check out the [example program](https://github.com/BinaryAlien/libssq/blob/main/example/example.c).

## Build

This project uses the [CMake](https://cmake.org/) build system generator.

### Example

* Produce a **static** library from the root of the repository using CMake.
```sh
cmake -B builddir
cmake --build builddir
```

* Produce a **dynamic** library from the root of the repository using CMake.
```sh
cmake -B builddir -DBUILD_SHARED_LIBS=1
cmake --build builddir
```

* (GNU/Linux) Compile the [example program](https://github.com/BinaryAlien/libssq/blob/main/example/example.c) using [GCC](https://gcc.gnu.org/).
```sh
gcc -std=c99 -Iinclude -o example example/example.c -Lbuilddir -lssq
```
