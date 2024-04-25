#include <VulkanEngine/Constants.h>
#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/StagedBuffer.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/Utilities.h>

#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

cxxopts::ParseResult setupProgramOptions(int argc, char **argv) {
  cxxopts::Options options("SimpleScene",
                           "Load an OBJ file and render using VulkanEngine");
  options.add_options()("o,obj", "Path to OBJ file",
                        cxxopts::value<std::string>())(
      "m,mtl", "Path where associated mtl file is",
      cxxopts::value<std::string>());

  return options.parse(argc, argv);
}

void moveCamera(std::shared_ptr<VulkanEngine::KeyboardInput> keyboard_input, std::shared_ptr<VulkanEngine::Camera> camera, float speed = 0.03) {
    // WASD keys move the camera in x, z directions, Z and X keys move the
    // camera in y direction.
    const auto key_w = keyboard_input->getLastKeyStatus(GLFW_KEY_W);
    const auto key_a = keyboard_input->getLastKeyStatus(GLFW_KEY_A);

    const auto key_s = keyboard_input->getLastKeyStatus(GLFW_KEY_S);
    const auto key_d = keyboard_input->getLastKeyStatus(GLFW_KEY_D);

    const auto key_z = keyboard_input->getLastKeyStatus(GLFW_KEY_Z);
    const auto key_x = keyboard_input->getLastKeyStatus(GLFW_KEY_X);

    Eigen::Vector3f camera_movement = {0.0f, 0.0f, 0.0f};

    if (key_a == VulkanEngine::KeyboardInput::PRESSED ||
        key_a == VulkanEngine::KeyboardInput::REPEAT) {
      camera_movement(0) += speed;
    }

    if (key_d == VulkanEngine::KeyboardInput::PRESSED ||
        key_d == VulkanEngine::KeyboardInput::REPEAT) {
      camera_movement(0) -= speed;
    }

    if (key_z == VulkanEngine::KeyboardInput::PRESSED ||
        key_z == VulkanEngine::KeyboardInput::REPEAT) {
      camera_movement(1) += speed;
    }

    if (key_x == VulkanEngine::KeyboardInput::PRESSED ||
        key_x == VulkanEngine::KeyboardInput::REPEAT) {
      camera_movement(1) -= speed;
    }

    if (key_w == VulkanEngine::KeyboardInput::PRESSED ||
        key_w == VulkanEngine::KeyboardInput::REPEAT) {
      camera_movement(2) += speed;
    }

    if (key_s == VulkanEngine::KeyboardInput::PRESSED ||
        key_s == VulkanEngine::KeyboardInput::REPEAT) {
      camera_movement(2) -= speed;
    }

    camera->setLookAt(camera->getLookAt() + camera_movement);
    camera->setTranform(
        Eigen::Affine3f(Eigen::Translation3f(camera_movement) *
                        Eigen::Affine3f(camera->getTransform()))
            .matrix());
}

int main(int argc, char **argv) {
  auto option_result = setupProgramOptions(argc, argv);

  // Create the GLFW window.
  std::shared_ptr<VulkanEngine::Window> window(
      new VulkanEngine::GLFWWindow(1280, 800, "SimpleScene", false));
  window->initialize();

  // Create the engine instance.
  auto vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  if (!vulkan_manager->initialize(window)) {
    return 1;
  }

  // Create a new scene.
  std::shared_ptr<VulkanEngine::Scene> scene(
      new VulkanEngine::Scene({window}));

  // Define a camera to view renderable objects.
  std::shared_ptr<VulkanEngine::Camera> camera(
    new VulkanEngine::Camera(
      {0.0f, 0.0f, 0.1f}, {0.0f, 1.0f, 0.0f}, 0.1f, 1000.0f, 45.0f,
      window->getFramebufferWidth(), window->getFramebufferHeight()));

  // Set initial camera transform.
  camera->setTranform(
      Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, 5.0f)).matrix());

  // Load an OBJ mesh and MTL file if configured.
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh;

  if (option_result["obj"].count()) {
    std::string obj_path = option_result["obj"].as<std::string>();
    std::string mtl_path;
    if (option_result["mtl"].count()) {
      mtl_path = option_result["mtl"].as<std::string>();
    }

    try {
      obj_mesh.reset(new VulkanEngine::OBJMesh(
          std::filesystem::path(obj_path), std::filesystem::path(mtl_path)));
    } catch (const std::exception &e) {
      std::cerr << "Failed to load obj mesh: " << e.what() << std::endl;
    } catch (...) {
      std::cerr << "An unknown exception occurred when loading obj mesh."
                << std::endl;
    }

    Eigen::Affine3f transform(Eigen::Translation3f(0.0f, 0.0f, 0.0f));
    transform *= Eigen::Scaling(0.5f);
    obj_mesh->setTranform(transform.matrix());

  } else {
    std::cout << "No OBJ file specified. Running empty scene."
              << std::endl;
  }

  scene->addChildren({{camera}, obj_mesh});

  const auto keyboard_input = window->getKeyboardInput();

  auto start_time = std::chrono::steady_clock::now();

  // Main scene loop.
  while (!window->shouldClose()) {
    if (keyboard_input.get()) {
      Eigen::Affine3f camera_transform =
          Eigen::Affine3f(camera->getTransform());

      if (obj_mesh.get()) {
        auto current_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds =
            current_time - start_time;

        start_time = std::chrono::steady_clock::now();

        auto matrix = obj_mesh->getTransform();
        Eigen::Transform<float, 3, Eigen::Affine> transform(matrix);

        // Rotate the object around its Y-axis.
        Eigen::AngleAxis<float> rotation(
            (VulkanEngine::Constants::pi<float>() / 4.0f) *
                elapsed_seconds.count(),
            Eigen::Vector3f::UnitY());
        transform.rotate(rotation);

        obj_mesh->setTranform(transform.matrix());

        moveCamera(keyboard_input, camera);
      }
    }

    scene->update();
    vulkan_manager->drawImage();
  }

  return 0;
}
