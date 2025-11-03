// Copyright (c) 2025 Michael Carlie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Window.h>

#include <iostream>
#include <memory>
#include <set>
#include <string>

VulkanEngine::Window::Window(uint32_t _width, uint32_t _height,
                             const std::string& _title, bool _full_screen)
    : width(_width),
      height(_height),
      title(_title),
      full_screen(_full_screen),
      size_changed(false) {}

VulkanEngine::Window::~Window() {}

std::shared_ptr<VulkanEngine::MouseInput> VulkanEngine::Window::getMouseInput()
    const {
  return mouse_input;
}

std::shared_ptr<VulkanEngine::KeyboardInput>
VulkanEngine::Window::getKeyboardInput() const {
  return keyboard_input;
}

void VulkanEngine::Window::setWidth(uint32_t _width) { width = _width; }

void VulkanEngine::Window::setHeight(uint32_t _height) { height = _height; }

void VulkanEngine::Window::setTitle(const std::string& new_title) {
  title = new_title;
}

uint32_t VulkanEngine::Window::getWidth() const { return width; }

uint32_t VulkanEngine::Window::getHeight() const { return height; }

uint32_t VulkanEngine::Window::getFramebufferWidth() const {
  return framebuffer_width;
}

uint32_t VulkanEngine::Window::getFramebufferHeight() const {
  return framebuffer_height;
}

const std::string& VulkanEngine::Window::getTitle() const { return title; }

bool VulkanEngine::Window::isFullScreen() const { return full_screen; }

bool VulkanEngine::Window::sizeHasChanged() const { return size_changed; }

void VulkanEngine::Window::mousePositionCallback(double xpos, double ypos) {
  if (mouse_input.get()) {
    mouse_input->setPosition(xpos, ypos);
  }
}

void VulkanEngine::Window::mouseScrollCallback(double xoffset, double yoffset) {
  if (mouse_input.get()) {
    mouse_input->setScrollOffset(xoffset, yoffset);
  }
}

void VulkanEngine::Window::mouseButtonPressedCallback(bool left_pressed,
                                                      bool right_pressed,
                                                      bool middle_pressed) {
  if (mouse_input.get()) {
    mouse_input->setButtons(left_pressed, right_pressed, middle_pressed);
  }
}

void VulkanEngine::Window::keyboardButtonPressedCallback(
    int scancode, const KeyboardInput::KeyInfo& key_info) {
  if (keyboard_input.get()) {
    keyboard_input->key_status_map[scancode] = key_info;
  }
}
