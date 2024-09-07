#ifndef OBJMESH_H
#define OBJMESH_H

#include "GraphicsPipeline.h"
#include <VulkanEngine/BoundingBox.h>
#include <VulkanEngine/GraphicsPipeline.h>
#include <VulkanEngine/MeshBase.h>
#include <VulkanEngine/SceneObject.h>
#include <VulkanEngine/UniformBuffer.h>

#include <filesystem>
#include <unordered_map>

namespace VulkanEngine {

/// A SceneObject which represents an OBJMesh.
class OBJMesh : public SceneObject {
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

#pragma pack(push, 1)
  struct MvpUbo {
    Eigen::Matrix4f model;
    Eigen::Matrix4f view;
    Eigen::Matrix4f projection;
  };

  struct Material {
    Eigen::Vector4f ambient = Eigen::Vector4f(0.8, 0.8, 0.8, 0.0);
    Eigen::Vector4f diffuse = Eigen::Vector4f(1.0, 1.0, 1.0, 0.0);
    Eigen::Vector4f specular = Eigen::Vector4f(1.0, 1.0, 1.0, 0.0);
  };
#pragma pack(pop)

  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void update(SceneState &scene_state) override;

  /// Load an obj file from the given path
  /// \param obj_path Path to the obj file.
  /// \param mtl_path Optional path to the mtl file.
  void loadOBJ(const char *obj_path, const char *mtl_path);

  /// \return Auto generated vertex shader for this OBJMesh.
  /// \param has_tex_coords Set to true if the obj has texture coordinates.
  const std::string getVertexShaderString(bool has_tex_coords, bool has_normals) const;

  /// \return Auto generated fragment shader for this OBJMesh.
  /// \param has_tex_coords Set to true if the obj has texture coordinates.
  const std::string getFragmentShaderString(bool has_texture, bool has_tex_coords, bool has_normals) const;

  /// Meshes composing this OBJMesh.
  std::vector<std::shared_ptr<MeshBase>> meshes;

  /// The shader to use for rendering the OBJMesh.
  std::vector<std::shared_ptr<Shader>> shaders;

  std::vector<std::shared_ptr<GraphicsPipeline>> graphics_pipelines;

  /// Model view projection uniform buffers for each frame in flight.
  std::vector<std::shared_ptr<UniformBuffer<MvpUbo>>> mvp_buffers;

  std::vector<std::vector<std::shared_ptr<UniformBuffer<Material>>>> material_buffers;

  /// Textures belonging to this mesh.
  std::vector<std::shared_ptr<Descriptor>> textures;

  /// True if the graphics pipeline has been updated.
  bool graphics_pipeline_updated;

  /// The OBJMesh's bounding box.
  BoundingBox<Eigen::Vector3f> bounding_box;
};

} // namespace VulkanEngine

#endif /* OBJMESH_H */
