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

#include <VulkanEngine/MouseInput.h>

#include <iostream>

VulkanEngine::MouseInput::MouseInput()
    : xpos(0),
      ypos(0),
      previous_x_pos(0),
      previous_y_pos(0),
      scroll_offset_x(0.0),
      scroff_offset_y(0.0),
      left_button_pressed(false),
      right_button_pressed(false),
      middle_button_pressed(false) {}

VulkanEngine::MouseInput::~MouseInput() {}

Eigen::Vector2d VulkanEngine::MouseInput::getPosition() { return {xpos, ypos}; }

Eigen::Vector2d VulkanEngine::MouseInput::getScrollOffset() {
  return {scroll_offset_x, scroff_offset_y};
}

bool VulkanEngine::MouseInput::leftButtonPressed() {
  return left_button_pressed;
}

void VulkanEngine::MouseInput::setPosition(double _xpos, double _ypos) {
  xpos = _xpos;
  ypos = _ypos;
}

void VulkanEngine::MouseInput::setScrollOffset(double xoffset, double yoffset) {
  scroll_offset_x = xoffset;
  scroff_offset_y = yoffset;
}

void VulkanEngine::MouseInput::setButtons(bool left, bool right, bool middle) {
  left_button_pressed = left;
  right_button_pressed = right;
  middle_button_pressed = middle;
}
