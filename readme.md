## About

This is a basic graphics engine using Vulkan API.

The project was started while I was working on refactoring an engine at a previous job and I wanted to get some insight into how modern graphics APIs work.

## Build
On Windows make sure you have Vulkan SDK installed.
https://www.lunarg.com/vulkan-sdk/

For MacOS (MoltenVK). Download the SDK and specify its location using the VULKAN_SDK environment variable.

Configure CMake. It will look for Vulkan SDK based on the VULKAN_SDK environment variable.

## Running
There is an example SimpleScene which demontrates how to use the API.

Use the `--obj` option to specify the obj file and `--mtl` to specify the mtl file if it's not in the same directory.
