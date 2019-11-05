#include <VulkanEngine/Scene.h>
#include <VulkanEngine/VulkanManager.h>

VulkanEngine::Scene::Scene() : state_instance() {

}

VulkanEngine::Scene::~Scene() {

}

void VulkanEngine::Scene::update() {
  VulkanManager::getInstance()->beginRenderPass();
  SceneObject::update( state_instance );
  VulkanManager::getInstance()->endRenderPass();
}

void VulkanEngine::Scene::update( SceneState& scene_state ) {
}
