#ifndef WINDOW_H
#define WINDOW_H

#include <VulkanEngine/KeyboardInput.h>
#include <VulkanEngine/MouseInput.h>

#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Provides an abstract interface for window implementations.
class Window {
public:
  /// Contructor.
  /// \param _width The desired initial width of the window.
  /// \param _height The desired initial height of the window.
  /// \param _title The title of the window.
  /// \param _full_screen If true the window will be initialized to full screen.
  Window(uint32_t _width, uint32_t _height, const std::string &_title,
         bool _full_screen);

  /// Desctructor.
  ~Window();

  /// Initialize the window.
  /// \return True on success.
  virtual bool initialize(bool invisible = false) = 0;

  /// \return A list of vulkan extensions required by the underlying windowing
  /// system.
  virtual const std::vector<const char *>
  getRequiredVulkanInstanceExtensions() const = 0;

  /// Create and return a vk::SurfaceKHR instance.
  /// \param instance The vk::Instance to create the surface for.
  /// \return The created vk::SurfaceKHR.
  virtual vk::SurfaceKHR createVkSurface(const vk::Instance &instance) = 0;

  /// Get a MouseInfo instance which retrieves values from this Window instance
  virtual std::shared_ptr<MouseInput> getMouseInput() const;

  /// Get keyboard input instance for this window.
  virtual std::shared_ptr<KeyboardInput> getKeyboardInput() const;

  /// Do anything needed to update the window.
  virtual void update() = 0;

  /// \return True if the window should close.
  virtual bool shouldClose() = 0;

  /// Sets the width of the window.
  /// \param _width The desired width.
  virtual void setWidth(uint32_t _width);

  /// Sets the height of the window.
  /// \param _height The desired height.
  virtual void setHeight(uint32_t _height);

  /// Set window title.
  /// \param new_title The new window title.
  virtual void setTitle(const std::string& new_title);

  /// \return The width of the window.
  uint32_t getWidth() const;

  /// \return The height of the window.
  uint32_t getHeight() const;

  /// \return The width of the window's framebuffer.
  uint32_t getFramebufferWidth() const;

  /// \return The width of the window's framebuffer.
  uint32_t getFramebufferHeight() const;

  /// \return The window's title.
  const std::string &getTitle() const;

  /// \return True if the window is in full screen mode.
  bool isFullScreen() const;

  /// \return True if the size has changed.
  bool sizeHasChanged() const;

protected:
  /// Callback which gets the current position of the mouse
  /// \param xpos The x position of the mouse
  /// \param ypos The y position of the mouse
  void mousePositionCallback(double xpos, double ypos);

  /// Callback which gets the current status of mouse buttons
  void mouseButtonPressedCallback(bool left_pressed, bool right_pressed,
                                  bool middle_pressed);

  void keyboardButtonPressedCallback(int scancode,
                                     const KeyboardInput::KeyInfo &key_info);

  /// The current width of the window.
  uint32_t width;

  /// The current height of the window.
  uint32_t height;

  /// The width of the window's framebuffer.
  uint32_t framebuffer_width;

  /// The height of the window's framebuffer.
  uint32_t framebuffer_height;

  /// The title of the window.
  std::string title;

  /// Whether or not the window is set to full screen.
  bool full_screen;

  /// If true the window or framebuffer size has changed.
  /// Reset when calling sizeHasChanged().
  bool size_changed;

  /// The MouseInput instance connected to this Window.
  std::shared_ptr<MouseInput> mouse_input;

  /// The KeyboardInput instance connect to this Window.
  std::shared_ptr<KeyboardInput> keyboard_input;
};

} // namespace VulkanEngine

#endif /* WINDOW_H */
