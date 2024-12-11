#include <VulkanEngine/RenderPass.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/VulkanManager.h>
#include <memory>

VulkanEngine::Scene::Scene(const std::vector<std::shared_ptr<Window>> &_windows)
    : windows(_windows) {}

VulkanEngine::Scene::~Scene() {}

void VulkanEngine::Scene::update() {
  if (!state_instance.get()) {
    state_instance.reset(new SceneState(*this));
  }

  for (const auto window : windows) {
    if (window.get() != nullptr) {
      window->update();
    }
  }
  VulkanManager::getInstance().getDefaultRenderPass()->begin();
  SceneObject::update(state_instance);
  VulkanManager::getInstance().getDefaultRenderPass()->end();
}

const std::shared_ptr<VulkanEngine::Window>
VulkanEngine::Scene::getActiveWindow() const {
  for (size_t i = 0; i < windows.size(); ++i) {
    return windows[i]; // TODO support multiple windows.
  }

  return std::shared_ptr<VulkanEngine::Window>();
}

void VulkanEngine::Scene::update(std::shared_ptr<SceneState> scene_state) {}
