#include <VulkanEngine/MouseInput.h>
#include <iostream>

VulkanEngine::MouseInput::MouseInput()
    : xpos(0), ypos(0), previous_x_pos(0), previous_y_pos(0),
      scroll_offset_x(0.0), scroff_offset_y(0.0),
      left_button_pressed(false), right_button_pressed(false),
      middle_button_pressed(false) {}

VulkanEngine::MouseInput::~MouseInput() {}

Eigen::Vector2d VulkanEngine::MouseInput::getPosition() {
  return {xpos, ypos};
}

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