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

#include <ExampleUtils.h>
#include <VulkanEngine/Constants.h>
#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/StagedBuffer.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/Utilities.h>

#include <algorithm>
#include <chrono>
#include <cxxopts.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

cxxopts::ParseResult setupProgramOptions(int argc, char** argv) {
  cxxopts::Options options("SimpleScene",
                           "Load an OBJ file and render using VulkanEngine");
  options.add_options()("o,obj", "Path to OBJ file",
                        cxxopts::value<std::string>())(
      "m,mtl", "Path where associated mtl file is",
      cxxopts::value<std::string>())("w,width", "Window width in pixels",
                                     cxxopts::value<unsigned>())(
      "h,height", "Window height in pixels", cxxopts::value<unsigned>());

  return options.parse(argc, argv);
}

int main(int argc, char** argv) {
  auto option_result = setupProgramOptions(argc, argv);

  unsigned window_width = 1280;
  unsigned window_height = 800;
  if (option_result.count("width")) {
    window_width = option_result["width"].as<unsigned>();
  }
  if (option_result.count("height")) {
    window_height = option_result["height"].as<unsigned>();
  }

  std::string title = "SimpleScene";

  // Create the GLFW window.
  auto window = std::make_shared<VulkanEngine::GLFWWindow>(
      window_width, window_height, title, false);
  if (!window->initialize()) {
    return 1;
  }

  // Create the engine singleton.
  auto& vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  if (!vulkan_manager.initialize(window)) {
    return 1;
  }

  // Create a new scene.
  auto windows = std::vector<std::shared_ptr<VulkanEngine::Window>>({window});
  auto scene = std::make_shared<VulkanEngine::Scene>(windows);

  // Define a camera to view renderable objects.
  auto camera = std::make_shared<VulkanEngine::Camera>(
      Eigen::Vector3f(0.0f, 0.0f, 0.1f),  // look at
      Eigen::Vector3f(0.0f, 1.0f, 0.0f),  // up vector
      0.1f,                               // z-near
      10.0f,                              // z-far
      45.0f,                              // fov
      window->getFramebufferWidth(), window->getFramebufferHeight());

  // Set initial camera transform.
  camera->setTransform(
      Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, 3.0f)).matrix());

  // Load an OBJ mesh and MTL file if configured.
  // Defaults to bunny.obj if no OBJ file specified.
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh;
  std::string obj_path = "assets/bunny.obj";
  std::string mtl_path = "";
  if (option_result["obj"].count() > 0) {
    obj_path = option_result["obj"].as<std::string>();
  }
  if (option_result["mtl"].count() > 0) {
    mtl_path = option_result["mtl"].as<std::string>();
  }

  try {
    auto obj_file_system_path = std::filesystem::path(obj_path);
    obj_mesh.reset(new VulkanEngine::OBJMesh(obj_file_system_path,
                                             std::filesystem::path(mtl_path)));
    title += " (" + obj_file_system_path.filename().string() + ")";
  } catch (const std::exception& e) {
    std::cerr << "Failed to load obj mesh: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "An unknown exception occurred when loading obj mesh."
              << std::endl;
    return 1;
  }

  // Set initial transform at center and scale based on bounding box.
  auto transform = Eigen::Affine3f::Identity();
  auto bounding_box = obj_mesh->getBoundingBox();
  auto bbox_size = bounding_box.max - bounding_box.min;
  auto center = (bounding_box.max + bounding_box.min) * 0.5f;
  auto mesh_scale = 1.5 / std::max({bbox_size(0), bbox_size(1), bbox_size(2)});
  transform.scale(mesh_scale);
  transform.translation() -= center * mesh_scale;
  obj_mesh->setTransform(transform.matrix());

  // Add the Camera and OBJMesh to the scene.
  scene->addChildren({camera, obj_mesh});

  const auto keyboard_input = window->getKeyboardInput();

  auto start_time = std::chrono::steady_clock::now();
  auto frame_rate_start_time = start_time;

  // Main scene loop.
  unsigned frame_count = 0;
  Eigen::Vector2d prev_mouse_position = {0.0, 0.0};
  while (!window->shouldClose()) {
    if (keyboard_input.get()) {
      Eigen::Affine3f camera_transform =
          Eigen::Affine3f(camera->getTransform());

      if (obj_mesh.get()) {
        auto current_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds =
            current_time - start_time;

        start_time = std::chrono::steady_clock::now();

        std::chrono::duration<double> frame_rate_elapsed_time =
            current_time - frame_rate_start_time;
        if (frame_rate_elapsed_time >= std::chrono::seconds(1)) {
          double frame_rate = static_cast<double>(frame_count) /
                              frame_rate_elapsed_time.count();
          window->setTitle(title + " " +
                           std::to_string(static_cast<int>(frame_rate)) +
                           " fps");
          frame_count = 0;
          frame_rate_start_time = std::chrono::steady_clock::now();
        }

        auto matrix = obj_mesh->getTransform();
        Eigen::Transform<float, 3, Eigen::Affine> transform(matrix);

        const auto& mouse_input = window->getMouseInput();
        const auto mouse_position = mouse_input->getPosition();

        if (mouse_input->leftButtonPressed()) {
          auto mouse_movement = -(mouse_position - prev_mouse_position) * 0.01;

          auto rotation_x =
              Eigen::AngleAxisf(mouse_movement.x(), Eigen::Vector3f::UnitY());
          auto rotation_y =
              Eigen::AngleAxisf(mouse_movement.y(), Eigen::Vector3f::UnitX());
          auto rotation = rotation_x * rotation_y;
          transform = transform.inverse();
          transform.rotate(rotation);
          transform = transform.inverse();
        }
        prev_mouse_position = mouse_position;

        const auto scroll_offset_2d = mouse_input->getScrollOffset();
        if (scroll_offset_2d.y() != 0.0) {
          Eigen::Vector3f position = camera->getTransform().block<3, 1>(0, 3);
          Eigen::Vector3f look_at = camera->getLookAt();
          Eigen::Vector3f direction = (look_at - position).normalized();
          float zoom_speed = 0.1f;
          float zoom_amount = -scroll_offset_2d.y() * zoom_speed;
          Eigen::Vector3f new_position = position + direction * zoom_amount;
          camera->setTransform(
              Eigen::Affine3f(Eigen::Translation3f(new_position)).matrix());
        }

        obj_mesh->setTransform(transform.matrix());

        moveCamera(keyboard_input, camera);
      }
    }

    scene->update();
    vulkan_manager.drawImage();
    ++frame_count;
  }

  return 0;
}
