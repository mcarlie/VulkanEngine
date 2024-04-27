#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/VulkanManager.h>

#include <iostream>

VulkanEngine::GLFWWindow::GLFWWindow(int _width, int _height,
                                     const std::string &_title,
                                     bool _full_screen)
    : Window(_width, _height, _title, _full_screen), glfw_window(nullptr) {
  glfwSetErrorCallback(errorCallback);
}

VulkanEngine::GLFWWindow::~GLFWWindow() {
  if (vk_surface) {
    VulkanManager::getInstance().getVkInstance().destroySurfaceKHR(vk_surface);
  }
  if (glfw_window != nullptr) {
    glfwDestroyWindow(glfw_window);
  }
  glfwTerminate();
}

void VulkanEngine::GLFWWindow::update() {
  size_changed = false;
  glfwPollEvents();
}

bool VulkanEngine::GLFWWindow::initialize(bool invisible) {
  if (!glfwInit()) {
    return false;
  }

  if (!glfwVulkanSupported()) {
    std::cerr << "Failed to initialize window. "
              << "Vulkan not supported by GLFW" << std::endl;
    return false;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  if (invisible) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }

  glfw_window = glfwCreateWindow(
      getWidth(), getHeight(), getTitle().c_str(),
      isFullScreen() ? glfwGetPrimaryMonitor() : nullptr, nullptr);

  if (!glfw_window) {
    return false;
  }

  glfwSetWindowUserPointer(glfw_window, this);
  glfwSetWindowSizeCallback(glfw_window, &windowResizeCallback);
  glfwSetFramebufferSizeCallback(glfw_window, &framebufferResizeCallback);

  glfwSetCursorPosCallback(glfw_window, &cursorPositionCallback);
  glfwSetMouseButtonCallback(glfw_window, &mouseButtonCallback);
  glfwSetKeyCallback(glfw_window, &keyCallback);

  mouse_input.reset(new MouseInput);
  keyboard_input.reset(new KeyboardInput);

  int fb_width, fb_height;
  glfwGetFramebufferSize(glfw_window, &fb_width, &fb_height);
  framebuffer_width = fb_width;
  framebuffer_height = fb_height;

  return true;
}

const std::vector<const char *>
VulkanEngine::GLFWWindow::getRequiredVulkanInstanceExtensions() const {
  uint32_t num_extensions;
  const char **extension_names;
  extension_names = glfwGetRequiredInstanceExtensions(&num_extensions);
  std::vector<const char *> extensions;
  for (uint32_t i = 0; i < num_extensions; ++i) {
    extensions.push_back(extension_names[i]);
  }
  return extensions;
}

bool VulkanEngine::GLFWWindow::shouldClose() {
  return glfwWindowShouldClose(glfw_window);
}

void VulkanEngine::GLFWWindow::setWidth(uint32_t _width) {
  if (glfw_window) {
    glfwSetWindowSize(glfw_window, static_cast<int>(_width), 0);
  } else {
    Window::setWidth(_width);
  }
}

void VulkanEngine::GLFWWindow::setHeight(uint32_t _height) {
  if (glfw_window) {
    glfwSetWindowSize(glfw_window, 0, static_cast<int>(_height));
  } else {
    Window::setHeight(_height);
  }
}

void VulkanEngine::GLFWWindow::setTitle(const std::string &new_title) {
  VulkanEngine::Window::setTitle(new_title);
  glfwSetWindowTitle(glfw_window, new_title.c_str());
}

void VulkanEngine::GLFWWindow::errorCallback(int error,
                                             const char *description) {
  std::cerr << "GLFW error: " << description << " error code:" << error
            << std::endl;
}

void VulkanEngine::GLFWWindow::windowResizeCallback(GLFWwindow *_glfw_window,
                                                    int _width, int _height) {
  GLFWWindow *window =
      static_cast<GLFWWindow *>(glfwGetWindowUserPointer(_glfw_window));
  if ((window->width != static_cast<uint32_t>(_width)) ||
      (window->height != static_cast<uint32_t>(_height))) {
    window->size_changed = true;
  }
  window->width = static_cast<uint32_t>(_width);
  window->height = static_cast<uint32_t>(_height);
}

void VulkanEngine::GLFWWindow::framebufferResizeCallback(
    GLFWwindow *_glfw_window, int _width, int _height) {
  GLFWWindow *window =
      static_cast<GLFWWindow *>(glfwGetWindowUserPointer(_glfw_window));
  if ((window->framebuffer_width != static_cast<uint32_t>(_width)) ||
      (window->framebuffer_height != static_cast<uint32_t>(_height))) {
    window->size_changed = true;
  }
  window->framebuffer_width = static_cast<uint32_t>(_width);
  window->framebuffer_height = static_cast<uint32_t>(_height);
}

void VulkanEngine::GLFWWindow::cursorPositionCallback(GLFWwindow *_glfw_window,
                                                      double xpos,
                                                      double ypos) {
  GLFWWindow *window =
      static_cast<GLFWWindow *>(glfwGetWindowUserPointer(_glfw_window));
  window->mousePositionCallback(xpos, ypos);
}

void VulkanEngine::GLFWWindow::mouseButtonCallback(GLFWwindow *_glfw_window,
                                                   int button, int action,
                                                   int mods) {
  GLFWWindow *window =
      static_cast<GLFWWindow *>(glfwGetWindowUserPointer(_glfw_window));
  bool left = false;
  bool right = false;
  bool middle = false;

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    left = action == GLFW_PRESS;
    right =
        glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    middle = glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_MIDDLE) ==
             GLFW_PRESS;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    left =
        glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    right = action == GLFW_PRESS;
    middle = glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_MIDDLE) ==
             GLFW_PRESS;
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    left =
        glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    right =
        glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    middle = action == GLFW_PRESS;
  } else {
    left =
        glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    right =
        glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    middle = glfwGetMouseButton(_glfw_window, GLFW_MOUSE_BUTTON_MIDDLE) ==
             GLFW_PRESS;
  }

  window->mouseButtonPressedCallback(left, middle, right);
}

void VulkanEngine::GLFWWindow::keyCallback(GLFWwindow *_glfw_window, int key,
                                           int scancode, int action, int mods) {
  GLFWWindow *window =
      static_cast<GLFWWindow *>(glfwGetWindowUserPointer(_glfw_window));

  KeyboardInput::KeyStatus key_status(KeyboardInput::KeyStatus::NO_STATUS);
  if (action == GLFW_PRESS) {
    key_status = KeyboardInput::KeyStatus::PRESSED;
  } else if (action == GLFW_RELEASE) {
    key_status = KeyboardInput::KeyStatus::RELEASED;
  } else if (action == GLFW_REPEAT) {
    key_status = KeyboardInput::KeyStatus::REPEAT;
  }

  KeyboardInput::KeyInfo key_info;
  key_info.key_id = key;
  key_info.status = key_status;

  window->keyboardButtonPressedCallback(scancode, key_info);
}

vk::SurfaceKHR VulkanEngine::GLFWWindow::getVkSurface() {
  if (!vk_surface) {
    VkResult err =
        glfwCreateWindowSurface(VulkanManager::getInstance().getVkInstance(),
                                glfw_window, nullptr, &vk_surface);
    if (err != VK_SUCCESS) {
      throw std::runtime_error("Could not create Vulkan Surface.");
    }
  }
  return vk_surface;
}