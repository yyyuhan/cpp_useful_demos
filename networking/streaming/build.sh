#!/usr/bin/env bash

export CMAKE_GENERATOR=Ninja
export INSTALL_DIR="$PWD/../.local"

mkdir -p cmake/build
pushd cmake/build
cmake -S ../.. -DCMAKE_PREFIX_PATH=$INSTALL_DIR
# CMAKE_PROGRAM_PATH: search path for protoc
#cmake -S .. -DCMAKE_PROGRAM_PATH=../.local/bin/
ninja
