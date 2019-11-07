#ifndef OBJMESH_H
#define OBJMESH_H

#include <VulkanEngine/SceneObject.h>
#include <VulkanEngine/MeshBase.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/GraphicsPipeline.h>

#include <unordered_map>
#include <filesystem>

namespace VulkanEngine {

  /// A SceneObject which represents an OBJMesh.
  class OBJMesh : public SceneObject, public GraphicsPipeline {

  public:

    /// Constructor.
    /// \param obj_file Path to obj file.
    /// \param mtl_file Path to mtl file location.
    OBJMesh(
      std::filesystem::path obj_file,
      std::filesystem::path mtl_path = "",
      const std::shared_ptr< Shader > _shader = std::shared_ptr< Shader >() );
    
    /// Destructor.
    virtual ~OBJMesh();

  private:

    struct MvpUbo {
      Eigen::Matrix4f model;
      Eigen::Matrix4f view;
      Eigen::Matrix4f projection;
    };
    
    /// \param scene_state Contains information about the current state of the scene.
    virtual void update( SceneState& scene_state ) override;

    void loadOBJ( const char* obj_path, const char* mtl_path );

    /// Meshes composing this OBJMesh.
    std::vector< std::shared_ptr< MeshBase > > meshes;

    /// The shader to use for rendering the OBJMesh.
    std::shared_ptr< Shader > shader;

    std::vector< std::shared_ptr< UniformBuffer< MvpUbo > > > mvp_buffers;
    
    /// Textures belonging to this mesh.
    std::unordered_map< std::string, std::shared_ptr< Descriptor > > textures;
    
    bool graphics_pipeline_updated;

  };

}

#endif /* OBJMESH_H */
