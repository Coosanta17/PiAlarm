# PiAlarm

Alarm clock software for raspberry pi zero 2 w (and others).

Currently unfinished. Once finished this file will have all the information on how this works. For now nothing.

## Binaries:

You can download the latest stable version from the releases tab - note this will only work for Rasperry Pi Linux systems.

You can also build from source on you Raspberry Pi - see [How to build](#how-to-build).

## How to build
1. Install Conan, CMake and a C++ compiler on your Raspberry Pi.
2. In the source directory install Conan dependencies with the command `conan install . --build=missing`.
3. Configure CMake: `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release`.
4. Build `cmake --build build` - optional add `-j` for multithreaded build. Some Raspberry Pis might run out of memory, if that is the case there is very little you can do - you can download prebuilt binaries or buy a stronger pi.
5. Binary is located in `./build`.

## Plan:
- [x] buzzer
- [x] button
- [ ] display
- [x] alarm function
- [x] api
- [x] frontend
- [ ] finished!!

