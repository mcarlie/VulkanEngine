This is an in-development engine using Vulkan and GLFW

On Windows make sure you have Vulkan SDK installed.
https://www.lunarg.com/vulkan-sdk/

For MacOS (MoltenVK). Download the SDK and specify it's location using the VULKAN_SDK environment variable (can be set in CMake)

This has not yet been tested on linux (though it's planned to support it in the future)

Run CMake and build the project.

There is an example SimpleScene which can be run and is used to test the engine.

It's hardcoded to load some obj file. You can download your own obj file and place
it in the models folder to test the engine. Also change the code to load your file.
It may be needed to change and recompile the shaders depending on if your file has textures or not.