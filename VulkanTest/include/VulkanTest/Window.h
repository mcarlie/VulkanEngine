#ifndef WINDOW_H
#define WINDOW_H

#include <vulkan/vulkan.hpp>

#include <string>

namespace VulkanTest {
  
  /// Provides an abstract interface for window implementations
  class Window {

  public:

    /// Contructor
    /// \param _width The desired initial width of the window
    /// \param _height The desired initial height of the window
    /// \param _title The title of the window
    /// \param _full_screen If true the window will be initialized to full screen
    Window( uint32_t _width, uint32_t _height, const std::string& _title, bool _full_screen );

    /// Desctructor
    ~Window();

    /// Initialize the window
    /// \return True on success
    virtual bool initialize() = 0;

    /// Create and return a vk::SurfaceKHR instance
    /// \param instance The vk::Instance to create the surface for
    /// \return The created vk::SurfaceKHR
    virtual vk::SurfaceKHR createVulkanSurface( const vk::Instance& instance ) = 0;

    /// \return A list of vulkan extensions required by the underlying windowing system
    virtual const std::vector< const char* > getRequiredVulkanInstanceExtensions() = 0;

    /// Do anything needed to update the window
    virtual void update() {};

    /// \return True if the window should close
    virtual bool shouldClose() = 0;

    /// Sets the width of the window
    virtual void setWidth( uint32_t _width );

    /// Sets the height of the window
    virtual void setHeight( uint32_t _height );

    /// \return The width of the window
    virtual uint32_t getWidth();

    /// \return The height of the window
    virtual uint32_t getHeight();

    /// \return The width of the window's framebuffer
    virtual uint32_t getFramebufferWidth();

    /// \return The width of the window's framebuffer
    virtual uint32_t getFramebufferHeight();

    /// \return The window's title
    virtual const std::string& getTitle();

    /// \return True if the window is in full screen mode
    virtual bool isFullScreen();

  protected:

    /// The current width of the window
    uint32_t width;

    /// The current height of the window
    uint32_t height;

    /// The width of the window's framebuffer
    uint32_t framebuffer_width;

    /// The height of the window's framebuffer
    uint32_t framebuffer_height;

    /// The title of the window
    std::string title;

    /// Whether or not the window is set to full screen
    bool full_screen;

  };

}

#endif /* WINDOW_H */