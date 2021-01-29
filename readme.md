This is an in-development engine using Vulkan and GLFW.

Right now there's support for loading an OBJ file with custom shaders and textures.

Currently working on some basic scene-graph functionality.

On Windows make sure you have Vulkan SDK installed.
https://www.lunarg.com/vulkan-sdk/

For MacOS (MoltenVK). Download the SDK and specify it's location using the VULKAN_SDK environment variable (can be set in CMake)

This has not yet been tested on linux (though it's planned to support it in the future)

Run CMake and build the project. Make sure that you open the project from CMake so that the proper environment is set.
Otherwise you have to specify the necessary environment variables yourself.

There is an example SimpleScene which can be run and is used to test the engine.

Use the -o option to specify the obj file and -m to specify the mtl file if it is in a different location.