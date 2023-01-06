# HomeAutomation PLC

## General Description and Features

This SoftPLC framework allows for developing applications using the cyclic execution pattern typically found in PLCs. The guiding principle is to provide all that's needed for the application logic (such as IO states) as variable values. Application developers can therefore fully focus on creating the actual application logic without having to deal with low-level system details.

Also see the introductory article on our website (more to come):
- https://honeytreelabs.com/posts/smart-home-requirements-and-architecture/

Supported featuers:

- Scheduler supporting an arbitrary number of tasks.
- Implementation of programs in C/C++ and Lua.
- I<sup>2</sup>C, ModBus, MQTT IO variables.
- A small standard library containing useful and tested logic blocks.
- A comprehensive build framework with multitude of tests.
- A statically linked binary with batteries included.

## Getting started

Requirements:

- [GNU Make](https://www.gnu.org/software/make/)
- [Docker Compose](https://docs.docker.com/compose/install/)
- [Python 3](https://www.python.org/)
- [CMake 3.20+](https://cmake.org/)
- [Ninja](https://ninja-build.org/)
- [GCC 8+](https://gcc.gnu.org/)
- [Valgrind](https://valgrind.org/)
- Optional: [crosstool-ng](https://crosstool-ng.github.io/)

The Conan package manager will be installed in a [Python 3 venv](https://docs.python.org/3/library/venv.html).

The following platforms are currently supported:

- Native (most likely this is x86_64)
- Raspberry Pi 4
- Raspberry Pi 2

In order to build the toolchain for the latter two, please refer to the directories beneath [cmake/toolchain/ct-ng](./cmake/toolchain/ct-ng). They contain the configurations for crosstool-ng. Find the documentation for this tool [here](https://crosstool-ng.github.io/docs/).

Building:

- `make conan-install`: install the conan package manager
- `make conan-install-deps-native`: build and install dependencies for native platform
- `make conan-install-deps-rpi4`: build and install dependencies for Raspberry Pi 4 (optional)
- `make conan-install-deps-rpi2`: build and install dependencies for Raspberry Pi 2 (optional)
- `make native`: build tests for native platform
- `make roof`: build PLC application for the roof Raspberry Pi 4 (optional)
- `make ground`: build PLC application for the ground Raspberry Pi 2 (optional)
- `make basement`: build PLC application for the basement Raspberry Pi 4 (optional)
- `make test`: build and run tests locally directly on the build platform

## Roadmap

The following list is a living document mentioning what is planned in the future.

### Implementation Improvements

- Docker based builds for easier porting/building for foreign platforms.
- Using the [doctest](https://github.com/doctest/doctest) testing framework instead of [Catch2](https://github.com/catchorg/Catch2) resulting in faster test builds and execution times.

### Integration of a Prometheus Client

For better analysis of what happened when and how often, we want to integrate a Prometheus Client, such as [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp).

### IEC 61131-3 Programs

In central Europe, PLCs used in typical industrial automation scenarios are programmed according to the IEC 61131-3 standard. Therefore, on the middle or long run, we want to implement an interpreter which is capable of executing such logic.

## References

I wrote a couple of articles about the protocols mentioned in this project. Unfortunately, they are in German. But most likely, it is possible to translate them with some available online tools:

- [Feldbussysteme unter Linux konfigurieren und einsetzen](https://www.linux-magazin.de/ausgaben/2018/04/feldbusse/), Rainer Poisel, Linux Magazin 04/2018
- [Hausautomatisierung auf Basis des MQTT-Protokolls](https://www.linux-magazin.de/ausgaben/2017/07/mqtt/), Rainer Poisel, Linux Magazin 07/2017
- [Hausautomatisierung mit I2C-Buskomponenten](https://www.linux-magazin.de/ausgaben/2016/12/i2c-bus/), Rainer Poisel, Linux Magazin 12/2016
