#include <VulkanEngine/MouseInput.h>

VulkanEngine::MouseInput::MouseInput()
    : xpos(0), ypos(0), previous_x_pos(0), previous_y_pos(0),
      left_button_pressed(false), right_button_pressed(false),
      middle_button_pressed(false) {}

VulkanEngine::MouseInput::~MouseInput() {}

void VulkanEngine::MouseInput::getPosition(double &x, double &y) {
  x = xpos;
  y = ypos;
}

void VulkanEngine::MouseInput::getPreviousPosition(double &x, double &y) {
  x = previous_x_pos;
  y = previous_y_pos;
}

bool VulkanEngine::MouseInput::leftButtonPressed() {
  return left_button_pressed;
}

void VulkanEngine::MouseInput::setPosition(double _xpos, double _ypos) {
  previous_x_pos = xpos;
  previous_y_pos = ypos;

  xpos = _xpos;
  ypos = _ypos;
}

void VulkanEngine::MouseInput::setButtons(bool left, bool right, bool middle) {
  left_button_pressed = left;
  right_button_pressed = right;
  middle_button_pressed = middle;
}