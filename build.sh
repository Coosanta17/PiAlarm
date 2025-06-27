#!/bin/bash
set -e

conan install . --build=missing

cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

cmake --build build -j 2
