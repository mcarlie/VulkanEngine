This is an in-development basic engine using Vulkan.

Right now there's support for loading an OBJ model file along with an MTL material file.

On Windows make sure you have Vulkan SDK installed.
https://www.lunarg.com/vulkan-sdk/

For MacOS (MoltenVK). Download the SDK and specify it's location using the VULKAN_SDK environment variable (can be set in CMake)

Run CMake and build the project.

There is an example SimpleScene which demontrates how to use the API.

Use the -o option to specify the obj file and -m to specify the mtl file if it is in a different location.
