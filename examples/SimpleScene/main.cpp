#include <VulkanEngine/Constants.h>
#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/StagedBuffer.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/Utilities.h>

#include <chrono>
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

cxxopts::ParseResult setupProgramOptions(int argc, char **argv) {
  cxxopts::Options options("SimpleScene",
                           "Load an OBJ file and render using VulkanEngine");
  options.add_options()
      ("o,obj", "Path to OBJ file", cxxopts::value<std::string>())
      ("m,mtl", "Path where associated mtl file is", cxxopts::value<std::string>())
      ("w,width", "Window width in pixels", cxxopts::value<unsigned>())
      ("h,height", "Window height in pixels", cxxopts::value<unsigned>());

  return options.parse(argc, argv);
}

void moveCamera(std::shared_ptr<VulkanEngine::KeyboardInput> keyboard_input, std::shared_ptr<VulkanEngine::Camera> camera, float speed = 0.03) {
    // A and D keys move the camera to the left and right
    // W and S keys move the camera forwards and backwards. 
    // Z and X keys move the camera up and down..
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
    camera->setTransform(
        Eigen::Affine3f(Eigen::Translation3f(camera_movement) *
                        Eigen::Affine3f(camera->getTransform()))
            .matrix());
}

int main(int argc, char **argv) {
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
  auto window = std::make_shared<VulkanEngine::GLFWWindow>(window_width, window_height, title, false);
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
    Eigen::Vector3f(0.0f, 0.0f, 0.1f), // look at
    Eigen::Vector3f(0.0f, 1.0f, 0.0f), // up vector
    0.1f,                              // z-near
    10.0f,                             // z-far
    45.0f,                             // fov
    window->getFramebufferWidth(),     // width 
    window->getFramebufferHeight()     // height
    );

  // Set initial camera transform.
  camera->setTransform(
      Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, 3.0f)).matrix());

  double mesh_scale = 1.0;

  // Load an OBJ mesh and MTL file if configured.
  // Without an OBJ file we still render an empty scene.
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh;
  if (option_result["obj"].count() > 0) {
    std::string obj_path = option_result["obj"].as<std::string>();
    std::string mtl_path;
    if (option_result["mtl"].count() > 0) {
      mtl_path = option_result["mtl"].as<std::string>();
    }

    try {
      auto obj_file_system_path = std::filesystem::path(obj_path);
      obj_mesh.reset(new VulkanEngine::OBJMesh(
          obj_file_system_path, std::filesystem::path(mtl_path)));
      title += " (" + obj_file_system_path.filename().string() + ")";
    } catch (const std::exception &e) {
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

  } else {
    std::cout << "No OBJ file specified. Running empty scene."
              << std::endl;
    title += " (Empty scene)";
  }

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

        std::chrono::duration<double> frame_rate_elapsed_time = current_time - frame_rate_start_time;
        if (frame_rate_elapsed_time >= std::chrono::seconds(1)) {
          double frame_rate = static_cast<double>(frame_count) / frame_rate_elapsed_time.count();
          window->setTitle(title + " " + std::to_string(static_cast<int>(frame_rate)) + " fps");
          frame_count = 0;
          frame_rate_start_time = std::chrono::steady_clock::now();;
        }

        auto matrix = obj_mesh->getTransform();
        Eigen::Transform<float, 3, Eigen::Affine> transform(matrix);

        const auto& mouse_input = window->getMouseInput();
        Eigen::Vector2d mouse_position; 
        mouse_input->getPosition(mouse_position.x(), mouse_position.y());

        if (mouse_input->leftButtonPressed()) {

          auto mouse_movement = -(mouse_position - prev_mouse_position) * 0.01;

          auto rotation_x = Eigen::AngleAxisf(mouse_movement.x(), Eigen::Vector3f::UnitY());
          auto rotation_y = Eigen::AngleAxisf(mouse_movement.y(), Eigen::Vector3f::UnitX());
          auto rotation = rotation_x * rotation_y;
          transform = transform.inverse();
          transform.rotate(rotation);
          transform = transform.inverse();

        }
        prev_mouse_position = mouse_position;

        Eigen::Vector3d scroll_offset = {0.0, 0.0, 0.0};
        mouse_input->getScrollOffset(scroll_offset.x(), scroll_offset.y());
        scroll_offset *= 0.1;
        scroll_offset.x() *= -1;
        camera->setLookAt(camera->getLookAt() + scroll_offset.cast<float>());
        camera->setTransform(
            Eigen::Affine3f(Eigen::Translation3f(scroll_offset.cast<float>()) *
                            Eigen::Affine3f(camera->getTransform()))
                .matrix());

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
