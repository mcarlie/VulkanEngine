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

#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/VulkanManager.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>

class EngineIntegrationTests : public ::testing::Test {
 protected:
  std::streambuf* cerr_backup;
  std::stringstream cerr_buffer;

  VulkanEngine::VulkanManager* vulkan_manager;
  std::shared_ptr<VulkanEngine::GLFWWindow> window;

  void SetUp() override {
    // Override cerr to read error messages.
    cerr_backup = std::cerr.rdbuf();
    std::cerr.rdbuf(cerr_buffer.rdbuf());

    window.reset(new VulkanEngine::GLFWWindow(1280, 800, "Test Window", false));
    ASSERT_TRUE(window->initialize(true));

    vulkan_manager = &VulkanEngine::VulkanManager::getInstance();
    ASSERT_TRUE(vulkan_manager->initialize(window));
  }

  void TearDown() override {
    std::cerr.rdbuf(cerr_backup);
    window.reset();
    vulkan_manager->resetInstance();
  }
};

TEST_F(EngineIntegrationTests, RenderEmptyScene) {
  std::shared_ptr<VulkanEngine::Scene> scene(new VulkanEngine::Scene({window}));

  scene->update();
  vulkan_manager->drawImage();
}

TEST_F(EngineIntegrationTests, CreateOBJMeshInvalidPath) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
      std::filesystem::path("path"), std::filesystem::path("path")));

  ASSERT_TRUE(cerr_buffer.str().find("Provided obj path path is invalid") !=
              std::string::npos);
}

TEST_F(EngineIntegrationTests, CreateOBJMeshBunny) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
      std::filesystem::path("./assets/bunny.obj"), std::filesystem::path("")));

  ASSERT_TRUE(cerr_buffer.str().find("Provided obj path path is invalid") ==
              std::string::npos);
}

TEST_F(EngineIntegrationTests, RenderOBJMeshBunny) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
      std::filesystem::path("./assets/bunny.obj"), std::filesystem::path("")));

  std::shared_ptr<VulkanEngine::Scene> scene(new VulkanEngine::Scene({window}));

  auto camera = std::make_shared<VulkanEngine::Camera>(
      Eigen::Vector3f(0.0f, 0.0f, 0.1f),  // look at
      Eigen::Vector3f(0.0f, 1.0f, 0.0f),  // up vector
      0.1f,                               // z-near
      10.0f,                              // z-far
      45.0f,                              // fov
      window->getFramebufferWidth(), window->getFramebufferHeight());

  scene->addChildren({obj_mesh, camera});

  scene->update();
  vulkan_manager->drawImage();
}

TEST_F(EngineIntegrationTests, CreateOBJMeshCapsule) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
      std::filesystem::path("./assets/capsule/capsule.obj"),
      std::filesystem::path("./assets/capsule/capsule.mtl")));

  std::shared_ptr<VulkanEngine::Scene> scene(new VulkanEngine::Scene({window}));

  auto camera = std::make_shared<VulkanEngine::Camera>(
      Eigen::Vector3f(0.0f, 0.0f, 0.1f),  // look at
      Eigen::Vector3f(0.0f, 1.0f, 0.0f),  // up vector
      0.1f,                               // z-near
      10.0f,                              // z-far
      45.0f,                              // fov
      window->getFramebufferWidth(), window->getFramebufferHeight());

  scene->addChildren({obj_mesh, camera});

  scene->update();
  vulkan_manager->drawImage();

  ASSERT_TRUE(cerr_buffer.str().find("Provided obj path path is invalid") ==
              std::string::npos);
}
