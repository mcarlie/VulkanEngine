#include <gtest/gtest.h>

#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/OBJMesh.h>

TEST(EngineIntegrationTests, CreateWindow) {
  std::shared_ptr<VulkanEngine::Window> window(
      new VulkanEngine::GLFWWindow(1280, 800, "SimpleScene", false));
  ASSERT_TRUE(window->initialize(true));
}

TEST(EngineIntegrationTests, CreateEngineInstance) {
  std::shared_ptr<VulkanEngine::Window> window(
      new VulkanEngine::GLFWWindow(1280, 800, "SimpleScene", false));
  ASSERT_TRUE(window->initialize(true));

  auto vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  ASSERT_TRUE(vulkan_manager->initialize(window));
}

TEST(EngineIntegrationTests, CreateScene) {
  std::shared_ptr<VulkanEngine::Window> window(
      new VulkanEngine::GLFWWindow(1280, 800, "SimpleScene", false));
  ASSERT_TRUE(window->initialize(true));

  std::vector<std::shared_ptr<VulkanEngine::Window>> window_list;
  window_list.push_back(window);

  std::shared_ptr<VulkanEngine::Scene> scene(
      new VulkanEngine::Scene(window_list));
}

TEST(EngineIntegrationTests, CreateCamera) {
  std::shared_ptr<VulkanEngine::Camera> camera;
  camera.reset(new VulkanEngine::Camera(
      {0.0f, 0.0f, 0.1f}, {0.0f, 1.0f, 0.0f}, 0.1f, 1000.0f, 45.0f,
      1280, 800));
}

TEST(EngineIntegrationTests, AddCameraToScene) {
  std::shared_ptr<VulkanEngine::Window> window(
      new VulkanEngine::GLFWWindow(1280, 800, "SimpleScene", false));
  ASSERT_TRUE(window->initialize(true));

  std::vector<std::shared_ptr<VulkanEngine::Window>> window_list;
  window_list.push_back(window);

  std::shared_ptr<VulkanEngine::Scene> scene(
      new VulkanEngine::Scene(window_list));

  std::vector<std::shared_ptr<VulkanEngine::SceneObject>> scene_children;

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
}

class OBJMeshIntegrationTest : public ::testing::Test {
protected:
  std::streambuf* cerr_backup;
  std::stringstream cerr_buffer;

  void SetUp() override {
    // Override cerr to read error messages.
    cerr_backup = std::cerr.rdbuf();
    std::cerr.rdbuf(cerr_buffer.rdbuf());
  }

  void TearDown() override {
    std::cerr.rdbuf(cerr_backup);
  }
};

TEST_F(OBJMeshIntegrationTest, CreateOBJMeshInvalidPath) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
    std::filesystem::path("path"), std::filesystem::path("path")));

  ASSERT_TRUE(cerr_buffer.str().find("Provided obj path path is invalid") != std::string::npos);
}

TEST_F(OBJMeshIntegrationTest, CreateOBJMeshBunny) {
  std::shared_ptr<VulkanEngine::OBJMesh> obj_mesh(new VulkanEngine::OBJMesh(
    std::filesystem::path("./assets/bunny.obj"), std::filesystem::path("")));

  ASSERT_TRUE(cerr_buffer.str().find("Provided obj path path is invalid") == std::string::npos);
}