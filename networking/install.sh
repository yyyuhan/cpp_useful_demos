#!/usr/bin/env bash

mkdir .local
export INSTALL_PREFIX="$PWD/.local"
export PATH="$INSTALL_PREFIX/bin:$PATH"
export CMAKE_GENERATOR=Ninja
export PB_VER=“v3.21.9”
export GRPC_VER=“v1.50.0”

# Install ProtoBuf
git clone --recurse-submodules -b $PB_VER --depth 1 https://github.com/google/protobuf.git
pushd protobuf
./autogen.sh
./configure --prefix=$INSTALL_PREFIX
make -j 4
make install
popd

# Install gRPC
git clone --recurse-submodules -b $GRPC_VER --depth 1 https://github.com/grpc/grpc.git
cmake -S grpc -B .build/grpc \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
    -DCMAKE_PREFIX_PATH=$INSTALL_PREFIX \
    -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DgRPC_PROTOBUF_PROVIDER=package \
    -DABSL_PROPAGATE_CXX_STD=ON
cmake --build .build/grpc --target install
