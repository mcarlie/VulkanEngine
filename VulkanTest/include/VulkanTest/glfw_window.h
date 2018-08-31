#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

#include <VulkanTest/window.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace VulkanTest {

  /// An implementation of VulkanTest::Window which uses the GLFW library
  class GLFW_Window : public Window {

  public:

    /// Contructor.
    /// \param _width The desired initial width of the window
    /// \param _height The desired initial height of the window
    /// \param _title The title of the window
    /// \param _full_screen If true the window will be initialized to full screen
    GLFW_Window( int width, int height, const std::string& title, bool _full_screen );
    
    /// Desctructor.
    ~GLFW_Window();

    /// Initialize the window
    virtual bool initialize();

    /// Create a Vulkan surface using the GLFW API 
    bool createVulkanSurface( const VkInstance& instance );

  private:

    /// Internal instance of the glfw window
    GLFWwindow* glfw_window;

    VkSurfaceKHR vulkan_surface;

    /// Callback which is called by glfw to indicate errors
    /// Prints the error to std::err
    static void errorCallback( int error, const char* description );

  };

}

#endif /* GLFW_WINDOW_H */