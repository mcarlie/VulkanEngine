#include <gtest/gtest.h>

#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/OBJMesh.h>
#include <memory>

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

    window.reset(
        new VulkanEngine::GLFWWindow(1280, 800, "Test Window", false));
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
  std::shared_ptr<VulkanEngine::Scene> scene(
      new VulkanEngine::Scene({window}));

  scene->update();
  vulkan_manager->drawImage();
}

TEST_F(EngineIntegrationTests, CreateOBJMeshInvalidPath) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
    std::filesystem::path("path"), std::filesystem::path("path")));

  ASSERT_TRUE(cerr_buffer.str().find("Provided obj path path is invalid") != std::string::npos);
}

TEST_F(EngineIntegrationTests, CreateOBJMeshBunny) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
    std::filesystem::path("./assets/bunny.obj"), std::filesystem::path("")));

  ASSERT_TRUE(cerr_buffer.str().find("Provided obj path path is invalid") == std::string::npos);
}

TEST_F(EngineIntegrationTests, RenderOBJMeshBunny) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
    std::filesystem::path("./assets/bunny.obj"), std::filesystem::path("")));

  std::shared_ptr<VulkanEngine::Scene> scene(
      new VulkanEngine::Scene({window}));

  auto camera = std::make_shared<VulkanEngine::Camera>(
    Eigen::Vector3f(0.0f, 0.0f, 0.1f), // look at
    Eigen::Vector3f(0.0f, 1.0f, 0.0f), // up vector
    0.1f,                              // z-near
    10.0f,                             // z-far
    45.0f,                             // fov
    window->getFramebufferWidth(),     // width 
    window->getFramebufferHeight()     // height
    );


  scene->addChildren({obj_mesh, camera});

  scene->update();
  vulkan_manager->drawImage();
}

TEST_F(EngineIntegrationTests, CreateOBJMeshCapsule) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
    std::filesystem::path("./assets/capsule/capsule.obj"), std::filesystem::path("./assets/capsule/capsule.mtl")));

  std::shared_ptr<VulkanEngine::Scene> scene(
      new VulkanEngine::Scene({window}));

  auto camera = std::make_shared<VulkanEngine::Camera>(
    Eigen::Vector3f(0.0f, 0.0f, 0.1f), // look at
    Eigen::Vector3f(0.0f, 1.0f, 0.0f), // up vector
    0.1f,                              // z-near
    10.0f,                             // z-far
    45.0f,                             // fov
    window->getFramebufferWidth(),     // width 
    window->getFramebufferHeight()     // height
    );


  scene->addChildren({obj_mesh, camera});

  scene->update();
  vulkan_manager->drawImage();

  ASSERT_TRUE(cerr_buffer.str().find("Provided obj path path is invalid") == std::string::npos);
}