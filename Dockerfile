FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y \
    build-essential \
    cmake \
    git \
    gdb \
    libglfw3 \
    libglfw3-dev \
    libeigen3-dev \
    libvulkan-dev \
    glslang-dev \
    glslang-tools \
    vulkan-validationlayers-dev \
    mesa-vulkan-drivers \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
