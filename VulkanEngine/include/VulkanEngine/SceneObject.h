#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <VulkanEngine/SceneState.h>

#include <vector>
#include <memory>

namespace VulkanEngine {

  /// Represents an object in a scene.
  class SceneObject {

  public:

    /// Contructor.
    SceneObject();

    /// Update this scene object.
    /// \param scene_state Contains information about the current state of the scene.
    void update( SceneState& scene_state );

    void addChildren( const std::vector< std::shared_ptr< SceneObject > >& _children );

    const Eigen::Matrix4f& getTransform();
    
    void setTranform( const Eigen::Matrix4f& _transform );
    
  protected:

    /// The transformation matrix of this SceneObject.
    /// By default it is set to identity.
    Eigen::Matrix4f transform;

  private:

    /// Called from update before updating children.
    /// \param scene_state Contains information about the current state of the scene.
    virtual void updateCallback( SceneState& scene_state );

    /// Contains any children of this scene object.
    std::vector< std::shared_ptr< SceneObject > > children;

  };

}

#endif /* SCENEOBJECT_H */
