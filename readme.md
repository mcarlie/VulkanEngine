## About

This is a scene-graph graphics engine based on Vulkan.

This project was started while I was working on creating an engine at a previous job and I wanted to get some insight into how modern graphics APIs work to prepare the engine for it.

The goal is to have a custom engine to use for exploring new graphical techniques.

Current short term goals are:
* Fully modulerize the backend (some functionality is missing such as configurable framebuffers)
* Write unit tests and integration tests
* Test on different hardware

The engine has been tested on MacOS 14 Sonoma, Ubuntu 23.03 and Windows 11. However I do not have access to a Windows 11 machine with Vulkan support so only know that it builds.

## Build

CMake is required for building.

### MacOS
Tested on MacOS 14 Sonoma with Clang 15.

```
brew install vulkan-tools glfw3 eigen

git clone https://github.com/mcarlie/VulkanEngine.git
cd VulkanEngine
mkdir build && cd build
cmake ..
cmake --build .
```

### Ubuntu
Ubuntu 23.04 or higher is required. Tested with GCC 12.3.0.

```
apt-get install -y \
    libglfw3 \
    libglfw3-dev \
    libeigen3-dev \
    libvulkan-dev \
    glslang-dev \
    glslang-tools \
    vulkan-validationlayers-dev \

git clone https://github.com/mcarlie/VulkanEngine.git
cd VulkanEngine
mkdir build && cd build
cmake ..
cmake --build .
```

### Windows
Tested on Windows 11 with MSBuild version 17.9.8.

You can install dependencies with [vcpkg](https://vcpkg.io).

Command Prompt:
```
./vcpkg install glfw3 eigen3 glslang vulkan
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>\scripts\buildsystems\vcpkg.cmake
cmake --build .
```

### Docker
A Dockerfile is provided which includes all dependencies and can be used to build.

```
docker build -f Dockerfile -t vulkan-engine .
docker run -v ./:/src vulkan-engine /bin/sh -c "cd /src && cmake -B build . && cd build && make"
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
