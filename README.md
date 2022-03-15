# libssq

☢️ An up-to-date [Source Server query protocol](https://developer.valvesoftware.com/wiki/Server_queries) library written in C.

If you like this library, please let me know by adding a ⭐ to the repository!

## Support

This library supports all of the undeprecated A2S queries:
* `A2S_INFO`
* `A2S_PLAYER`
* `A2S_RULES`

It has **no dependencies** and is meant to be built on both **MS/Windows** and **UNIX-like** operating systems.

However, it does **not** currently support **Goldsource** and **compressed** responses.

## Documentation

The [repository wiki](https://github.com/BinaryAlien/libssq/wiki) pages contain all of the documentation for you to easily understand and make use of `libssq`.

In order to quickly get started, check out the [**Getting started**](https://github.com/BinaryAlien/libssq/wiki/Getting-started) wiki page, as well as the [**example program**](https://github.com/BinaryAlien/libssq/blob/main/example/example.c).

## Build

This library is meant to be built using [**CMake**](https://cmake.org/).

### Example

* Building the library from the root of the repository.

```sh
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

* Building the [example program](https://github.com/BinaryAlien/libssq/blob/main/example/example.c) using `gcc` from the root of the repository.

```sh
$ gcc -Iinclude example/example.c -o ssq -Lbuild -lssq
$ ./ssq
usage: ./ssq hostname [port]
```
