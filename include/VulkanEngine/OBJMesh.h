#ifndef OBJMESH_H
#define OBJMESH_H

#include <VulkanEngine/BoundingBox.h>
#include <VulkanEngine/GraphicsPipeline.h>
#include <VulkanEngine/MeshBase.h>
#include <VulkanEngine/SceneObject.h>
#include <VulkanEngine/UniformBuffer.h>

#include <filesystem>
#include <unordered_map>

namespace VulkanEngine {

/// A SceneObject which represents an OBJMesh.
class OBJMesh : public SceneObject, public GraphicsPipeline {
public:
  /// Constructor.
  /// \param obj_file Path to obj file.
  /// \param mtl_file Path to mtl file location.
  OBJMesh(std::filesystem::path obj_file, std::filesystem::path mtl_path = "",
          const std::shared_ptr<Shader> _shader = std::shared_ptr<Shader>());

  /// Destructor.
  virtual ~OBJMesh();

  /// \return The OBJMesh's bounding box.
  const BoundingBox<Eigen::Vector3f> &getBoundingBox() const;

private:
  struct MvpUbo {
    Eigen::Matrix4f model;
    Eigen::Matrix4f view;
    Eigen::Matrix4f projection;
  };

  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void update(SceneState &scene_state) override;

  /// Load an obj file from the given path
  /// \param obj_path Path to the obj file.
  /// \param mtl_path Optional path to the mtl file.
  void loadOBJ(const char *obj_path, const char *mtl_path);

  /// \return Auto generated vertex shader for this OBJMesh.
  /// \param has_tex_coords Set to true if the obj has texture coordinates.
  const std::string getVertexShaderString(bool has_tex_coords) const;

  /// \return Auto generated fragment shader for this OBJMesh.
  /// \param has_tex_coords Set to true if the obj has texture coordinates.
  const std::string getFragmentShaderString(bool has_tex_coords) const;

  /// Meshes composing this OBJMesh.
  std::vector<std::shared_ptr<MeshBase>> meshes;

  /// The shader to use for rendering the OBJMesh.
  std::shared_ptr<Shader> shader;

  /// Model view projection uniform buffers for each frame in flight.
  std::vector<std::shared_ptr<UniformBuffer<MvpUbo>>> mvp_buffers;

  /// Textures belonging to this mesh.
  std::unordered_map<std::string, std::shared_ptr<Descriptor>> textures;

  /// True if the graphics pipeline has been updated.
  bool graphics_pipeline_updated;

  /// The OBJMesh's bounding box.
  BoundingBox<Eigen::Vector3f> bounding_box;
};

} // namespace VulkanEngine

#endif /* OBJMESH_H */
