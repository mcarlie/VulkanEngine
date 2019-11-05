#ifndef SCENE_H
#define SCENE_H

#include <VulkanEngine/SceneObject.h>

#include <memory>

namespace VulkanEngine {

  class Scene : public SceneObject {

  public:

    /// Constructor.
    Scene();

    /// Destructor.
    virtual ~Scene();
    
    void update();
    
  private:

    /// \param scene_state Contains information about the current state of the scene.
    virtual void update( SceneState& scene_state ) override;
    
    SceneState state_instance;

  };

}

#endif /* SCENE_H */
