## About

This is a scene-graph graphics engine based on Vulkan.

This project was started while I was working on creating an engine at a previous job and I wanted to get some insight into how modern graphics APIs work to prepare the engine for it.

The goal is to have a custom engine to use as a base for further personal projects in graphics.

Current short term goals are:
* Fully modulerize the backend (some functionality is missing such as configurable framebuffers)
* Write unit tests and integration tests
* Test on different hardware
* Come up with a better name

The engine has been tested on MacOS 14 Sonoma, Ubuntu 23.04 and Windows 11. However I do not have access to a Windows 11 machine with Vulkan support so only know that it builds.

## Build

CMake is required for building.

First clone and enter the project directory.

```
git clone https://github.com/mcarlie/VulkanEngine.git
cd VulkanEngine
```

### MacOS
Tested on MacOS 14 Sonoma with Clang 15.

```
brew install vulkan-tools glfw3 eigen && \
mkdir build && \
cmake -S . -B build && \
cmake --build build
```

### Ubuntu
Ubuntu 22.04 or higher is required.

```
sudo apt-get install -y \
    libglfw3 \
    libglfw3-dev \
    libeigen3-dev \
    libvulkan-dev \
    glslang-dev \
    glslang-tools \
    vulkan-validationlayers-dev && \
mkdir build && \
cmake -S . -B build && \
cmake --build build
```

### Windows
Visual Studio is required. Tested on Windows 11 with Visual Studio 2022 MSBuild version 17.9.8.

You can install dependencies with [vcpkg](https://vcpkg.io).

Command Prompt:
```
git clone https://github.com/microsoft/vcpkg.git && \
.\vcpkg\bootstrap-vcpkg.bat && \
.\vcpkg\vcpkg integrate install && \
.\vcpkg\vcpkg install glfw3 eigen3 glslang vulkan && \
$env:VULKAN_SDK="$PWD\vcpkg\installed\x64-windows" && \
mkdir build && \
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=$PWD\vcpkg\scripts\buildsystems\vcpkg.cmake && \
cmake --build build
```

### Docker
A Dockerfile is provided which includes all dependencies and can be used to build for Ubuntu 22.04.

```
docker build -f Dockerfile -t vulkan-engine .
docker run -v $(pwd):/src vulkan-engine /bin/sh -c "cmake -B /src/build /src && cmake --build /src/build"
```

Build output will be available in the `build` folder on the host.

## Running
There is an example SimpleScene which demontrates how to use the API.

Use the `--obj` option to specify the obj file and `--mtl` to specify the mtl file (if it's not in the same directory).

### Inside a Docker container
You can run the engine inside a Docker container with software rendering (expect very slow performance).

```
docker run -v ./build:/build -e DISPLAY=host.docker.internal:0 vulkan-engine /build/SimpleScene
```

[Docker supports GPU rendering with NVidia GPUs.](https://docs.docker.com/config/containers/resource_constraints/#expose-gpus-for-use)

## Documentation
Documentation can be generated with Doxygen:

```
doxygen doc/Doxyfile
```
