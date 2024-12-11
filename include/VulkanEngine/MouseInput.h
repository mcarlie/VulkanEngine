// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_MOUSEINPUT_H_
#define INCLUDE_VULKANENGINE_MOUSEINPUT_H_

#include <Eigen/Eigen>

namespace VulkanEngine {

class GLFWWindow;

/// Class which can be used to get input from the mouse
class MouseInput {
  friend class Window;

 public:
  /// Constructor.
  MouseInput();

  /// Destructor.
  ~MouseInput();

  /// Get the current position of the mouse
  /// \return X and Y coordinates of the mouse position.
  Eigen::Vector2d getPosition();

  /// Get the current scroll offset mouse
  /// \return Horizontal and vertical scroll offsets.
  Eigen::Vector2d getScrollOffset();

  /// \return True if the left mouse button is currently pressed
  bool leftButtonPressed();

 private:
  /// Set the position of the mouse
  /// \param _xpos The x position of the mouse
  /// \param _ypos The y position of the mouse
  void setPosition(double _xpos, double _ypos);

  /// Set the position of the mouse
  /// \param _xpos The x position of the mouse
  /// \param _ypos The y position of the mouse
  void setScrollOffset(double xoffset, double yoffset);

  /// Set the current status of the mouse buttons
  void setButtons(bool left, bool right, bool middle);

  /// The x position of the mouse
  double xpos;

  /// The y position of the mouse
  double ypos;

  /// Current scroll offset along x
  double scroll_offset_x;

  /// Current scroll offset along y
  double scroff_offset_y;

  /// The previous x position of the mouse
  double previous_x_pos;

  /// The previous y position of the mouse
  double previous_y_pos;

  /// True if the left button is pressed
  bool left_button_pressed;

  /// True if the right button is pressed
  bool right_button_pressed;

  /// True if the middle button is pressed
  bool middle_button_pressed;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_MOUSEINPUT_H_
