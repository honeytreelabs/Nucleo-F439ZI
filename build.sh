#!/usr/bin/env bash

mkdir build \
    && cd build \
    && cmake \
         -DCMAKE_BUILD_TYPE=Debug \
         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/toolchain.stm32f4.cmake \
         -GNinja \
         .. \
    && ninja -v \
    && cd .. \
    && ln -sf build/compile_commands.json compile_commands.json
