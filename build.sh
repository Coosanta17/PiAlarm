#!/bin/bash
set -e

conan install . --build=missing
cmake . -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake   -DCMAKE_BUILD_TYPE=Release

cd ./build
cmake --build .. -j $(nproc)
