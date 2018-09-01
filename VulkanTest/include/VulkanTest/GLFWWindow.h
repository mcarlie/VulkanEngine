#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#include <VulkanTest/Window.h>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace VulkanTest {

  /// An implementation of VulkanTest::Window which uses the GLFW library
  class GLFWWindow : public Window {

  public:

    /// Contructor.
    /// \param _width The desired initial width of the window
    /// \param _height The desired initial height of the window
    /// \param _title The title of the window
    /// \param _full_screen If true the window will be initialized to full screen
    GLFWWindow( int width, int height, const std::string& title, bool _full_screen );
    
    /// Desctructor.
    ~GLFWWindow();

    /// Initialize the window
    virtual bool initialize();

    virtual void update();

    /// Create a Vulkan surface using the GLFW API 
    bool createVulkanSurface( const VkInstance& instance );

    /// Returns true if the window should close
    virtual bool shouldClose();

  private:

    /// Internal instance of the glfw window
    GLFWwindow* glfw_window;

    /// Vulkan surface. Use createVulkanSurface to initialize
    VkSurfaceKHR vulkan_surface;

    /// Callback which is called by glfw to indicate errors
    /// Prints the error to std::err
    static void errorCallback( int error, const char* description );

  };

}

#endif /* GLFWWINDOW_H */