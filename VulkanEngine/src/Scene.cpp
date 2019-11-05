#include <VulkanEngine/Scene.h>
#include <VulkanEngine/VulkanManager.h>

VulkanEngine::Scene::Scene( const std::vector< std::shared_ptr< Window > >& _windows ) : windows( _windows ) {
}

VulkanEngine::Scene::~Scene() {
}

void VulkanEngine::Scene::update() {
  
  if( !state_instance.get() ) {
    state_instance.reset( new SceneState( *this ) );
  }
  
  windows[0]->update(); /// TODO
  VulkanManager::getInstance()->beginRenderPass();
  SceneObject::update( *state_instance.get() );
  VulkanManager::getInstance()->endRenderPass();
  
}

const std::shared_ptr< VulkanEngine::Window >& VulkanEngine::Scene::getActiveWindow() const {
  return windows[0]; // TODO
}

void VulkanEngine::Scene::update( SceneState& scene_state ) {
}
