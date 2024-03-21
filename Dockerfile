# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

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

# Copy your project files into the Docker container
# COPY . .

# Uncomment these lines if you want to build your project during the Docker image build
# RUN cmake .
# RUN make

# Command to run your application
# CMD ["./your_application"]