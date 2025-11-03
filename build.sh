#!/bin/bash

BUILD_TYPE="Release"

if ! command -v nix &> /dev/null; then
    echo "ERROR: Nix is not installed. Please install Nix from https://nixos.org/download.html and try again."
    exit 1
fi

if [ "$1" != "" ]; then
    if [ "$1" == "Debug" ]; then
        BUILD_TYPE="Debug"
    elif [ "$1" == "Release" ]; then
        BUILD_TYPE="Release"
    else
        echo "ERROR: Invalid argument. Use 'Debug' or 'Release'."
        exit 1
    fi
fi

nix-shell --run "
  mkdir -p build && \
  cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE && \
  cmake --build build --config $BUILD_TYPE
"
