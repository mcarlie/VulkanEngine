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
    Window( int _width, int _height, const std::string& _title, bool _full_screen );

    /// Desctructor
    ~Window();

    /// Initialize the window
    virtual bool initialize() = 0;

    /// Create and return a vk::SurfaceKHR instance
    virtual vk::SurfaceKHR createVulkanSurface( const vk::Instance& instance ) = 0;

    /// Return a list of vulkan extensions required by the underlying windowing system
    virtual const std::vector< const char* > getRequiredVulkanInstanceExtensions() = 0;

    /// Do anything needed to update the window
    virtual void update() {};

    /// Returns true if the window should close
    virtual bool shouldClose() = 0;

    /// Get the width of the window
    virtual int getWidth();

    /// Get the height of the window
    virtual int getHeight();

    /// Get the window's title
    virtual const std::string& getTitle();

    /// Returns true if the window is in full screen mode
    virtual bool isFullScreen();

  protected:

    /// The current width of the window
    int width;

    /// The current height of the window
    int height;

    /// The title of the window
    std::string title;

    /// Whether or not the window is set to full screen
    bool full_screen;

  };

}

#endif /* WINDOW_H */