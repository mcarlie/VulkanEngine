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
    /// \return True if initialization succeded without error
    virtual bool initialize();

    /// Do anything needed to update the window.
    virtual void update();

    /// Create a Vulkan surface using the GLFW API
    /// \param instance The vk::Instance to use with the window
    /// \return The created vulkan surface instance
    virtual vk::SurfaceKHR createVulkanSurface( const vk::Instance& instance );

    /// \return The required vulkan extensions for using GLFW
    virtual const std::vector< const char* > getRequiredVulkanInstanceExtensions();

    /// \return True if the window should close
    virtual bool shouldClose();

    /// Sets the width of the window
    /// \param _width The width value to set
    virtual void setWidth( uint32_t _width );

    /// Sets the height of the window
    /// \param _height The height value to set
    virtual void setHeight( uint32_t _height );

  private:

    /// Internal instance of the glfw window
    GLFWwindow* glfw_window;

    /// Callback which is called by glfw to indicate errors
    /// Prints the error to std::err
    /// \param error The error code
    /// \param description Text describing the error
    static void errorCallback( int error, const char* description );

    /// Callback which is called when the window's size has changed
    /// \param _glfw_window The window which was resized
    /// \param _width The new width of the window
    /// \param _height The new height of the window
    static void windowResizeCallback( GLFWwindow* _glfw_window, int _width, int _height );

    /// Callback which is called when the window's framebuffer size has changed
    /// \param _glfw_window The window which was resized
    /// \param _width The new width of the framebuffer
    /// \param _height The new height of the framebuffer
    static void framebufferResizeCallback( GLFWwindow* _glfw_window, int _width, int _height );

    /// Callback which is called when the mouse is over the window
    /// \param _glfw_window The window which recieved the event
    /// \param x_pos The position of the mouse along the x-axis relative to the top left
    /// \param y_pos The position of the mouse along the y-axis relative to the top left
    static void cursorPositionCallback( GLFWwindow* _glfw_window, double x_pos, double y_pos );

  };

}

#endif /* GLFWWINDOW_H */