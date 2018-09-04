#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#include <VulkanTest/Window.h>

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
    GLFWWindow( int _width, int _height, const std::string& _title, bool _full_screen );
    
    /// Desctructor.
    ~GLFWWindow();

    /// Initialize the window
    virtual bool initialize();

    /// Do anything needed to update the window.
    virtual void update();

    /// Create a Vulkan surface using the GLFW API
    /// \param instance The vk::Instance to use with the window
    virtual vk::SurfaceKHR createVulkanSurface( const vk::Instance& instance );

    /// \return The required vulkan extensions for using GLFW
    virtual const std::vector< const char* > getRequiredVulkanInstanceExtensions();

    /// \return True if the window should close
    virtual bool shouldClose();

  private:

    /// Internal instance of the glfw window
    GLFWwindow* glfw_window;

    /// Callback which is called by glfw to indicate errors
    /// Prints the error to std::err
    /// \param error The error code
    /// \param description Text describing the error
    static void errorCallback( int error, const char* description );

  };

}

#endif /* GLFWWINDOW_H */