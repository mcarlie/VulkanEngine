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
#include <vector>


int main(int argc, char **argv) {

  cxxopts::Options options("SimpleScene",
                           "Load an OBJ file and render using VulkanEngine");
  options.add_options()("o,obj", "Path to OBJ file",
                        cxxopts::value<std::string>())(
      "m,mtl", "Path where associated mtl file is",
      cxxopts::value<std::string>());

  auto option_result = options.parse(argc, argv);

  std::shared_ptr<VulkanEngine::Window> window(
      new VulkanEngine::GLFWWindow(1280, 800, "SimpleScene", false));
  window->initialize();

  auto vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  vulkan_manager->initialize(window);

  std::vector<std::shared_ptr<VulkanEngine::Window>> window_list;
  window_list.push_back(window);

  std::shared_ptr<VulkanEngine::Scene> scene(
      new VulkanEngine::Scene(window_list));

  std::vector<std::shared_ptr<VulkanEngine::SceneObject>> scene_children;

  std::shared_ptr<VulkanEngine::MouseInput> mouse_input =
      window->getMouseInput();

  std::shared_ptr<VulkanEngine::Camera> camera;
  camera.reset(new VulkanEngine::Camera(
      {0.0f, 0.0f, 0.1f}, {0.0f, 1.0f, 0.0f}, 0.1f, 1000.0f, 45.0f,
      window->getFramebufferWidth(), window->getFramebufferHeight()));

  camera->setTranform(
      Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, 5.0f)).matrix());

  std::shared_ptr<VulkanEngine::SceneObject> camera_container(
      new VulkanEngine::SceneObject());
  camera_container->addChildren({camera});

  scene_children.push_back({camera_container});

  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh;

  if (option_result["obj"].count()) {
    std::string obj_path = option_result["obj"].as<std::string>();
    std::string mtl_path;
    if (option_result["mtl"].count()) {
      mtl_path = option_result["mtl"].as<std::string>();
    }

    obj_mesh.reset(new VulkanEngine::OBJMesh(
        std::filesystem::path(obj_path), std::filesystem::path(mtl_path)));

    Eigen::Affine3f transform(Eigen::Translation3f(0.0f, 0.0f, 0.0f));
    transform *= Eigen::Scaling(0.5f);
    obj_mesh->setTranform(transform.matrix());

    scene_children.push_back(obj_mesh);
  } else {
    std::cout << "Warning: No OBJ file specified. Running empty scene."
              << std::endl;
  }

  scene->addChildren(scene_children);

  const auto &keyboard_input = window->getKeyboardInput();

  const float camera_move_speed = 0.03;

  auto start_time = std::chrono::steady_clock::now();

  while (!window->shouldClose()) {

    if (keyboard_input.get()) {

      Eigen::Affine3f camera_transform =
          Eigen::Affine3f(camera->getTransform());

      const auto key_left = keyboard_input->getLastKeyStatus(GLFW_KEY_LEFT);
      const auto key_right = keyboard_input->getLastKeyStatus(GLFW_KEY_RIGHT);

      const auto key_up = keyboard_input->getLastKeyStatus(GLFW_KEY_UP);
      const auto key_down = keyboard_input->getLastKeyStatus(GLFW_KEY_DOWN);

      float camera_rotation_x = 0.0f;
      float camera_rotation_y = 0.0f;

      if (key_left == VulkanEngine::KeyboardInput::PRESSED ||
          key_left == VulkanEngine::KeyboardInput::REPEAT) {
        camera_rotation_x += camera_move_speed;
      }

      if (key_right == VulkanEngine::KeyboardInput::PRESSED ||
          key_right == VulkanEngine::KeyboardInput::REPEAT) {
        camera_rotation_x -= camera_move_speed;
      }

      if (key_up == VulkanEngine::KeyboardInput::PRESSED ||
          key_up == VulkanEngine::KeyboardInput::REPEAT) {
        camera_rotation_y -= camera_move_speed;
      }

      if (key_down == VulkanEngine::KeyboardInput::PRESSED ||
          key_down == VulkanEngine::KeyboardInput::REPEAT) {
        camera_rotation_y += camera_move_speed;
      }

      if (camera_rotation_x != 0.0f || camera_rotation_y != 0.0f) {
        Eigen::AngleAxisf camera_rot_x(camera_rotation_x,
                                       camera->getUpVector().normalized());

        Eigen::Vector3f camera_position = camera_transform.translation();
        Eigen::Vector3f look_at = camera->getLookAt();

        Eigen::Vector3f look_at_to_pos = camera_position - look_at;

        look_at_to_pos = camera_rot_x * look_at_to_pos;

        Eigen::AngleAxisf camera_rot_y(
            camera_rotation_y,
            (look_at_to_pos.cross(camera->getUpVector())).normalized());

        look_at_to_pos = camera_rot_y * look_at_to_pos;

        camera->setTranform(
            Eigen::Affine3f(Eigen::Translation3f(look_at_to_pos + look_at))
                .matrix());
      }

      const auto key_w = keyboard_input->getLastKeyStatus(GLFW_KEY_W);
      const auto key_a = keyboard_input->getLastKeyStatus(GLFW_KEY_A);

      const auto key_s = keyboard_input->getLastKeyStatus(GLFW_KEY_S);
      const auto key_d = keyboard_input->getLastKeyStatus(GLFW_KEY_D);

      const auto key_z = keyboard_input->getLastKeyStatus(GLFW_KEY_Z);
      const auto key_x = keyboard_input->getLastKeyStatus(GLFW_KEY_X);

      Eigen::Vector3f camera_movement = {0.0f, 0.0f, 0.0f};

      if (key_a == VulkanEngine::KeyboardInput::PRESSED ||
          key_a == VulkanEngine::KeyboardInput::REPEAT) {
        camera_movement(0) += camera_move_speed;
      }

      if (key_d == VulkanEngine::KeyboardInput::PRESSED ||
          key_d == VulkanEngine::KeyboardInput::REPEAT) {
        camera_movement(0) -= camera_move_speed;
      }

      if (key_z == VulkanEngine::KeyboardInput::PRESSED ||
          key_z == VulkanEngine::KeyboardInput::REPEAT) {
        camera_movement(1) += camera_move_speed;
      }

      if (key_x == VulkanEngine::KeyboardInput::PRESSED ||
          key_x == VulkanEngine::KeyboardInput::REPEAT) {
        camera_movement(1) -= camera_move_speed;
      }

      if (key_w == VulkanEngine::KeyboardInput::PRESSED ||
          key_w == VulkanEngine::KeyboardInput::REPEAT) {
        camera_movement(2) += camera_move_speed;
      }

      if (key_s == VulkanEngine::KeyboardInput::PRESSED ||
          key_s == VulkanEngine::KeyboardInput::REPEAT) {
        camera_movement(2) -= camera_move_speed;
      }

      auto current_time = std::chrono::steady_clock::now();
      std::chrono::duration<double> elapsed_seconds = current_time - start_time;

      start_time = std::chrono::steady_clock::now();

      auto matrix = obj_mesh->getTransform();
      Eigen::Transform<float, 3, Eigen::Affine> transform(matrix);

      Eigen::AngleAxis<float> rotation((M_PI / 4) * elapsed_seconds.count(), Eigen::Vector3f::UnitY());

      // Apply the rotation to the transform
      transform.rotate(rotation);

      obj_mesh->setTranform(transform.matrix());

      camera->setLookAt(camera->getLookAt() + camera_movement);
      camera->setTranform(
          Eigen::Affine3f(Eigen::Translation3f(camera_movement) *
                          Eigen::Affine3f(camera->getTransform()))
              .matrix());
    }

    scene->update();
    vulkan_manager->drawImage();
  }

  return 0;
}
