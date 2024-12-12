## About

This is a simple graphics engine based on Vulkan.

This project was started to get some insight into how modern graphics APIs work.

![Screenshot 2024-09-07 233550](https://github.com/user-attachments/assets/f33099f1-d741-430c-a44b-0980b3e26e47)

## Documentation
[Doxygen output](https://mcarlie.github.io/VulkanEngine/)

## Build

### Nix (MacOS, Linux)

If you have Nix installed you can build the engine with the `build.sh` script.

### Windows

On Windows you can run the `build.bat` script.

### Manual build
<details>
  <summary>Click to expand</summary>

CMake version 3.16 or higher is required for building.

### MacOS
Tested on MacOS 14 Sonoma with Clang 15. Dependencies can be installed with [Homebrew](https://brew.sh/)

```
brew install vulkan-tools glfw3 eigen
mkdir build
cmake -S . -B build
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
mkdir build
cmake -S . -B build
cmake --build build
```

### Windows
Visual Studio is required. Tested on Windows 11 with Visual Studio 2022 MSBuild version 17.9.8.

You can install dependencies with [vcpkg](https://vcpkg.io).

PowerShell:
```
git clone https://github.com/microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg integrate install
.\vcpkg\vcpkg install glfw3 eigen3 glslang vulkan
$env:VULKAN_SDK="$PWD\vcpkg\installed\x64-windows"
mkdir build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$PWD\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build build
```
</details>

</break>

## Running
There is an example SimpleScene which demontrates how to use the API.

```
./build/examples/SimpleScene --obj assets/bunny.obj
```

Use the `--obj` option to specify the obj file and `--mtl` to specify the mtl file (if it's not in the same directory).

### Inside the Docker container
You can run the engine inside the Docker container with software rendering (expect very slow performance).

The following command is for MacOS. After building inside the container run:
```
docker run -v ./build:/build -e DISPLAY=host.docker.internal:0 vulkan-engine /build/examples/SimpleScene
```

[Docker supports GPU rendering with NVidia GPUs.](https://docs.docker.com/config/containers/resource_constraints/#expose-gpus-for-use)

## Tests
Tests can be enabled with the BUILD_TESTS CMake option.

```
cmake -S . -B build -DBUILD_TESTS=ON
```

After building, run tests with ctest.

```
ctest --test-dir build/tests
```

Tests are also run by Github Actions for each commit.
