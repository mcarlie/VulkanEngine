// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MOUSEINPUT_H
#define MOUSEINPUT_H

namespace VulkanEngine;
{

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
    /// \param [out] x The x position
    /// \param [out] y The y position
    void getPosition(double &x, double &y);

    /// Get the current scroll offset mouse
    /// \param [out] x The x scroll offset.
    /// \param [out] y The y scroll offset.
    void getScrollOffset(double &x, double &y);

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

#endif /* MOUSEINPUT_H */