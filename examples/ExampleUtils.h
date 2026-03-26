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

#ifndef EXAMPLES_EXAMPLEUTILS_H_
#define EXAMPLES_EXAMPLEUTILS_H_

#include <GLFW/glfw3.h>
#include <VulkanEngine/Camera.h>
#include <VulkanEngine/KeyboardInput.h>

#include <Eigen/Eigen>
#include <memory>

/// Move camera based on keyboard input.
/// Controls:
/// - W/S: forward/back
/// - A/D: left/right
/// - Z/X: up/down
/// \param keyboard_input The keyboard input handler.
/// \param camera The camera to move.
/// \param speed Movement speed factor (default 0.03).
inline void moveCamera(
    std::shared_ptr<VulkanEngine::KeyboardInput> keyboard_input,
    std::shared_ptr<VulkanEngine::Camera> camera, float speed = 0.03f) {
  const auto key_w = keyboard_input->getLastKeyStatus(GLFW_KEY_W);
  const auto key_a = keyboard_input->getLastKeyStatus(GLFW_KEY_A);
  const auto key_s = keyboard_input->getLastKeyStatus(GLFW_KEY_S);
  const auto key_d = keyboard_input->getLastKeyStatus(GLFW_KEY_D);
  const auto key_z = keyboard_input->getLastKeyStatus(GLFW_KEY_Z);
  const auto key_x = keyboard_input->getLastKeyStatus(GLFW_KEY_X);

  Eigen::Vector3f camera_movement = {0.0f, 0.0f, 0.0f};

  if (key_a == VulkanEngine::KeyboardInput::PRESSED ||
      key_a == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(0) -= speed;
  }
  if (key_d == VulkanEngine::KeyboardInput::PRESSED ||
      key_d == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(0) += speed;
  }
  if (key_z == VulkanEngine::KeyboardInput::PRESSED ||
      key_z == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(1) -= speed;
  }
  if (key_x == VulkanEngine::KeyboardInput::PRESSED ||
      key_x == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(1) += speed;
  }
  if (key_w == VulkanEngine::KeyboardInput::PRESSED ||
      key_w == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(2) -= speed;
  }
  if (key_s == VulkanEngine::KeyboardInput::PRESSED ||
      key_s == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(2) += speed;
  }

  camera->setLookAt(camera->getLookAt() + camera_movement);
  camera->setTransform(Eigen::Affine3f(Eigen::Translation3f(camera_movement) *
                                       Eigen::Affine3f(camera->getTransform()))
                           .matrix());
}

#endif  // EXAMPLES_EXAMPLEUTILS_H_
