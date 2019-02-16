#ifndef OBJMESH_H
#define OBJMESH_H

#include <VulkanEngine/SceneObject.h>
#include <VulkanEngine/MeshBase.h>
#include <VulkanEngine/UniformBuffer.h>

#include <unordered_map>

namespace VulkanEngine {

  /// A SceneObject which represents an OBJMesh.
  class OBJMesh : public SceneObject {

  public:

    /// Constructor.
    /// \param obj_file Path to obj file.
    /// \param mtl_file Path to mtl file location.
    OBJMesh( const std::string& obj_file, const std::string& mtl_path = "", const std::shared_ptr< Shader > _shader = std::shared_ptr< Shader >() );
    
    /// Destructor.
    virtual ~OBJMesh();

  private:

    struct MvpUbo {
      Eigen::Matrix4f model;
      Eigen::Matrix4f view;
      Eigen::Matrix4f projection;
    };
    
    /// Called from update before updating children.
    /// \param scene_state Contains information about the current state of the scene.
    virtual void updateCallback( SceneState& scene_state );

    void loadOBJ( const char* obj_path, const char* mtl_path );

    /// Meshes composing this OBJMesh.
    std::vector< std::shared_ptr< MeshBase > > meshes;

    /// The shader to use for rendering the OBJMesh.
    std::shared_ptr< Shader > shader;

    std::vector< std::shared_ptr< UniformBuffer< MvpUbo > > > mvp_buffers;
    
    /// Textures belonging to this mesh
    std::unordered_map< std::string, std::shared_ptr< Descriptor > > textures;

  };

}

#endif /* OBJMESH_H */
