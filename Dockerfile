# Use Ubuntu 22.04 as the base image
FROM ubuntu:23.04

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Update and install dependencies
RUN apt-get update
RUN apt-get install -y \
    build-essential \
    cmake \
    wget \
    git \
    gdb \
    software-properties-common \
    libglfw3 \
    libglfw3-dev \
    libeigen3-dev \
    libvulkan-dev \
    vulkan-validationlayers-dev \
    mesa-vulkan-drivers \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Set the working directory in the container
WORKDIR /usr/src/app

# COPY . .

# RUN cmake .
# RUN make

# CMD ["./your_application"]