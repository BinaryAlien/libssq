# Test suite

This is the test suite of `libssq`. It depends on [Criterion](https://github.com/Snaipe/Criterion).

## Building

This test suite is meant to be built using [CMake](https://cmake.org/).

### Example

* Building from the `tests` folder.

```sh
$ pwd
~/libssq/tests
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

## Running

* Run the `tests` executable from the `tests` directory.

```sh
$ pwd
~/libssq/tests
$ ./build/tests
```
