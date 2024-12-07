// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VulkanEngine/Window.h>

namespace VulkanEngine {

/// An implementation of VulkanEngine::Window which uses the GLFW library.
class GLFWWindow : public Window {
 public:
  /// Contructor.
  /// \param _width The desired initial width of the window.
  /// \param _height The desired initial height of the window.
  /// \param _title The title of the window.
  /// \param _full_screen If true the window will be initialized to full screen.
  GLFWWindow(int _width, int _height, const std::string &_title,
             bool _full_screen);

  /// Desctructor.
  virtual ~GLFWWindow();

  /// Initialize the window.
  /// \param invisible If set to true the window will be invisible.
  /// \return True if initialization succeded without error.
  virtual bool initialize(bool invisible = false);

  /// Do anything needed to update the window.
  virtual void update();

  /// \return The required vulkan extensions for using GLFW.
  virtual const std::vector<const char *> getRequiredVulkanInstanceExtensions()
      const;

  /// \return True if the window should close.
  virtual bool shouldClose();

  /// Sets the width of the window.
  /// \param _width The width value to set.
  virtual void setWidth(uint32_t _width);

  /// Sets the height of the window.
  /// \param _height The height value to set.
  virtual void setHeight(uint32_t _height);

  /// Set window title.
  /// \param new_title The new window title.
  virtual void setTitle(const std::string &new_title);

  /// Get the Vulkan surface defined for this window.
  /// \return The vk::SurfaceKHR.
  virtual vk::SurfaceKHR getVkSurface();

 private:
  /// Internal instance of the glfw window.
  GLFWwindow *glfw_window;

  /// Vulkan surface created by this window.
  VkSurfaceKHR vk_surface;

  /// Callback which is called by glfw to indicate errors.
  /// Prints the error to std::err.
  /// \param error The error code.
  /// \param description Text describing the error.
  static void errorCallback(int error, const char *description);

  /// Callback which is called when the window's size has changed.
  /// \param _glfw_window The window which was resized.
  /// \param _width The new width of the window.
  /// \param _height The new height of the window.
  static void windowResizeCallback(GLFWwindow *_glfw_window, int _width,
                                   int _height);

  /// Callback which is called when the window's framebuffer size has changed.
  /// \param _glfw_window The window which was resized.
  /// \param _width The new width of the framebuffer.
  /// \param _height The new height of the framebuffer.
  static void framebufferResizeCallback(GLFWwindow *_glfw_window, int _width,
                                        int _height);

  /// Callback which gives the current position of the mouse.
  /// \param _glfw_window The window which recieved the event.
  /// \param xpos The x position of the mouse.
  /// \param ypos The y position of the mouse.
  static void cursorPositionCallback(GLFWwindow *_glfw_window, double xpos,
                                     double ypos);

  /// Callback for storing scroll value.
  /// \param _glfw_window The window which recieved the event.
  /// \param xoffset The scroll offset along x.
  /// \param yoffset The scroll offset along y.
  static void scrollCallback(GLFWwindow *_glfw_window, double xoffset,
                             double yoffset);

  /// Callback which gives which buttons are currently clicked on the mouse.
  /// \param _glfw_window The window which recieved the event.
  /// \param button The button which has an event.
  /// \param action The action performed on the button.
  /// \param mods The modifier buttons pressed.
  static void mouseButtonCallback(GLFWwindow *_glfw_window, int button,
                                  int action, int mods);

  /// Callback which gives which buttons are currently pressed on the keyboard.
  /// \param _glfw_window The window which recieved the event.
  /// \param key GLFW key id.
  /// \param scancode scanecode id.
  /// \param action The action performed on the button.
  /// \param mods The modifier buttons pressed.
  static void keyCallback(GLFWwindow *_glfw_window, int key, int scancode,
                          int action, int mods);
};

}  // namespace VulkanEngine

#endif /* GLFWWINDOW_H */
