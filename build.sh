#!/bin/bash

if ! command -v nix &> /dev/null; then
    echo "ERROR: Nix is not installed. Please install Nix from https://nixos.org/download.html and try again."
    exit 1
fi

nix-shell --run "
  mkdir -p build && \
  cmake -S . -B build && \
  cmake --build build
"