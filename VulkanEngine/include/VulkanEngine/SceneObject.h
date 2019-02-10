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

    /// Add child SceneObject instances which will inherit the transformation of this SceneObject.
    /// \param _children The children to add.
    void addChildren( const std::vector< std::shared_ptr< SceneObject > >& _children );

    /// Get the current local transformation matrix.
    /// \return The current local transformation matrix
    const Eigen::Matrix4f& getTransform();
    
    /// Set the current local transformation matrix.
    /// \param _transform The desired local transformation matrix.
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
