#include <VulkanEngine/Mesh.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/Utilities.h>
#include <VulkanEngine/VulkanManager.h>
#include <limits>
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <chrono>

namespace OBJMeshInternal {

template <typename IndexType>
std::shared_ptr<VulkanEngine::MeshBase>
getShape(const tinyobj::shape_t &shape, const tinyobj::attrib_t &attrib, bool& has_tex_coord, bool& has_normals) {
  using Vertex = std::tuple<const Eigen::Vector3f &, const Eigen::Vector3f &,
                            const Eigen::Vector2f &>;
  std::unordered_map<Vertex, size_t> unique_vertices;

  const auto default_normal = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
  const auto default_texcoord = Eigen::Vector2f(0.0f, 0.0f);

  std::vector<Eigen::Vector3f> positions;
  positions.reserve(shape.mesh.indices.size());

  std::vector<IndexType> indices;
  indices.reserve(shape.mesh.indices.size());

  std::vector<Eigen::Vector3f> normals;
  normals.reserve(shape.mesh.indices.size());

  std::vector<Eigen::Vector2f> texcoords;
  texcoords.reserve(shape.mesh.indices.size());

  Eigen::Vector3f max_position = {std::numeric_limits<float>::min(),
                                  std::numeric_limits<float>::min(),
                                  std::numeric_limits<float>::min()},
                  min_position = {std::numeric_limits<float>::max(),
                                  std::numeric_limits<float>::max(),
                                  std::numeric_limits<float>::max()};

  for (size_t i = 0; i < shape.mesh.indices.size(); ++i) {
    positions.emplace_back(
        attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 0],
        attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 1],
        attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 2]);

    // Calculate bounding box.
    auto &position = positions.back();
    if (position.x() > max_position.x()) {
      max_position.x() = position.x();
    }
    if (position.y() > max_position.y()) {
      max_position.y() = position.y();
    }
    if (position.z() > max_position.z()) {
      max_position.z() = position.z();
    }

    if (position.x() < min_position.x()) {
      min_position.x() = position.x();
    }
    if (position.y() < min_position.y()) {
      min_position.y() = position.y();
    }
    if (position.z() < min_position.z()) {
      min_position.z() = position.z();
    }

    has_normals = false;
    if (shape.mesh.indices[i].normal_index > -1) {
      has_normals = true;
      normals.emplace_back(
          attrib.normals[3 * shape.mesh.indices[i].normal_index + 0],
          attrib.normals[3 * shape.mesh.indices[i].normal_index + 1],
          attrib.normals[3 * shape.mesh.indices[i].normal_index + 2]);
    }

    has_tex_coord = false;
    if (shape.mesh.indices[i].texcoord_index > -1) {
      has_tex_coord = true;
      texcoords.emplace_back(
          attrib.texcoords[2 * shape.mesh.indices[i].texcoord_index + 0],
          1.0f -
              attrib.texcoords[2 * shape.mesh.indices[i].texcoord_index + 1]);
    }

    Vertex vertex = std::forward_as_tuple(
        positions.back(), normals.empty() ? default_normal : normals.back(),
        texcoords.empty() ? default_texcoord : texcoords.back());

    if (unique_vertices.count(vertex) == 0) {
      unique_vertices[vertex] = positions.size() - 1;
    } else {
      positions.pop_back();
      if (!normals.empty()) {
        normals.pop_back();
      }
      if (!texcoords.empty()) {
        texcoords.pop_back();
      }
    }

    indices.push_back(static_cast<IndexType>(unique_vertices[vertex]));
  }

  using MeshType = VulkanEngine::Mesh<Eigen::Vector3f, IndexType,
                                      Eigen::Vector3f, Eigen::Vector2f>;

  std::shared_ptr<VulkanEngine::VertexAttribute<Eigen::Vector3f>>
      position_attribute(new VulkanEngine::VertexAttribute<Eigen::Vector3f>(
          positions.data(), positions.size(), 0, vk::Format::eR32G32B32Sfloat));

  std::shared_ptr<VulkanEngine::IndexAttribute<IndexType>> index_attribute(
      new VulkanEngine::IndexAttribute<IndexType>(indices.data(),
                                                  indices.size()));

  typename MeshType::template AttributeContainer<Eigen::Vector3f>
      normal_attribute;
  if (!normals.empty()) {
    normal_attribute.emplace_back(
        new VulkanEngine::VertexAttribute<Eigen::Vector3f>(
            normals.data(), normals.size(), 1, vk::Format::eR32G32B32Sfloat));
  }

  typename MeshType::template AttributeContainer<Eigen::Vector2f>
      texcoord_attribute;
  if (!texcoords.empty()) {
    texcoord_attribute.emplace_back(
        new VulkanEngine::VertexAttribute<Eigen::Vector2f>(
            texcoords.data(), texcoords.size(), 2, vk::Format::eR32G32Sfloat));
  }

  // AttributeContainer for additional attributes (normals and texcoords)
  std::tuple<typename MeshType::template AttributeContainer<Eigen::Vector3f>,
             typename MeshType::template AttributeContainer<Eigen::Vector2f>>
      additional_attributes(normal_attribute, texcoord_attribute);

  MeshType *mesh = new MeshType();
  mesh->setPositions(position_attribute);
  mesh->setIndices(index_attribute);
  mesh->setAttributes(additional_attributes);
  mesh->setBoundingBox(max_position, min_position);
  return std::shared_ptr<VulkanEngine::MeshBase>(mesh);
}

} // namespace OBJMeshInternal

VulkanEngine::OBJMesh::OBJMesh(std::filesystem::path obj_file,
                               std::filesystem::path mtl_path,
                               const std::shared_ptr<Shader> _shader) //TODO support custom shader.
    : SceneObject(),
      graphics_pipeline_updated(false), bounding_box() {
  std::error_code obj_file_error;
  if (!std::filesystem::exists(obj_file, obj_file_error)) {
    std::cerr << "Provided obj path " + (obj_file.string()) +
                     " is invalid. Error code: "
              << obj_file_error.value() << " " << obj_file_error.message()
              << std::endl;
    return;
  }

  std::error_code mtl_path_error;
  if (!mtl_path.empty()) {
    if (!std::filesystem::exists(mtl_path, mtl_path_error)) {
      std::cerr << "Provided mtl path " + mtl_path.string() +
                       " is invalid. Error code: "
                << mtl_path_error.value() << " " << mtl_path_error.message()
                << std::endl;
    }
    mtl_path /= ""; // Appends a / on POSIX systems. tinyobj doesn't recognize
                    // the path otherwise
  } else {
    auto obj_file_copy = obj_file;
    mtl_path = obj_file_copy.remove_filename();
  }

  // Load mesh
  loadOBJ(obj_file.string().c_str(), mtl_path.string().c_str());

  // Transfer mesh vertex data to GPU
  for (const auto &m : meshes) {
    m->transferBuffers();
  }
}

VulkanEngine::OBJMesh::~OBJMesh() {}

const BoundingBox<Eigen::Vector3f> &
VulkanEngine::OBJMesh::getBoundingBox() const {
  return bounding_box;
}

void VulkanEngine::OBJMesh::update(SceneState &scene_state) {
  MvpUbo ubo_data;
  ubo_data.projection = scene_state.getProjectionMatrix();
  ubo_data.view = scene_state.getViewMatrix();
  ubo_data.model = scene_state.getTotalTransform();

  for (auto &ub : mvp_buffers) {
    ub->updateBuffer(&ubo_data, sizeof(ubo_data));
  }

  auto &vulkan_manager = VulkanManager::getInstance();

  const auto window = scene_state.getScene().getActiveWindow();
  if (graphics_pipeline_updated) {
    graphics_pipeline_updated = !window->sizeHasChanged();
  }

  if (!graphics_pipeline_updated) {
    graphics_pipelines.clear();
    for (size_t i = 0; i < meshes.size(); ++i) {
      int32_t width = window->getFramebufferWidth();
      int32_t height = window->getFramebufferHeight();
      graphics_pipelines.push_back(std::shared_ptr<GraphicsPipeline>(new GraphicsPipeline()));
      graphics_pipelines.back()->setViewPort(0, 0, static_cast<float>(width), static_cast<float>(height),
                  0.0f, 1.0f);
      graphics_pipelines.back()->setScissor(0, 0, width, height);
      graphics_pipelines.back()->createGraphicsPipeline(meshes[i], shaders[i]);
    }
  }
  if (window.get() != nullptr) {
    for (size_t i = 0; i < meshes.size(); ++i) {

      graphics_pipelines[i]->bindPipeline();

      auto current_command_buffer = vulkan_manager.getCurrentCommandBuffer();

      meshes[i]->bindVertexBuffers(current_command_buffer);
      meshes[i]->bindIndexBuffer(current_command_buffer);
      shaders[i]->bindDescriptorSet(
          current_command_buffer,
          static_cast<uint32_t>(vulkan_manager.getCurrentFrame()));

      meshes[i]->draw(current_command_buffer);
    }
  }

  graphics_pipeline_updated = true;

  SceneObject::update(scene_state);
}

void VulkanEngine::OBJMesh::loadOBJ(const char *obj_path,
                                    const char *mtl_path) {
  auto begin = std::chrono::system_clock::now();

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  // TODO Get rid of need to triangulate using primitive restart
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, obj_path, mtl_path,
                        true)) {
    throw std::runtime_error(
        "Could not load obj file: " + std::string(obj_path) + ", " + err);
  }

  bounding_box.max = {std::numeric_limits<float>::min(),
                      std::numeric_limits<float>::min(),
                      std::numeric_limits<float>::min()};
  bounding_box.min = {std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max()};

  mvp_buffers.resize(VulkanManager::getInstance().getFramesInFlight());
  for (auto &ub : mvp_buffers) {
    ub.reset(new VulkanEngine::UniformBuffer<MvpUbo>(0));
  }

  material_buffers.resize(VulkanManager::getInstance().getFramesInFlight());

  for (size_t i = 0; i < shapes.size(); ++i) {
    bool has_tex_coord = false;
    bool has_normals = false;
    // We represent the indices using uint16_t unless that is not sufficient
    // given the number of indices
    if (shapes[i].mesh.indices.size() > std::numeric_limits<uint16_t>::max()) {
      meshes.push_back(OBJMeshInternal::getShape<uint32_t>(shapes[i], attrib, has_tex_coord, has_normals));
    } else {
      meshes.push_back(OBJMeshInternal::getShape<uint16_t>(shapes[i], attrib, has_tex_coord, has_normals));
    }

    // Calculate the bbox for the whole OBJ
    const auto &mesh_bbox = meshes.back()->getBoundingBox<Eigen::Vector3f>();
    if (mesh_bbox.max.x() > bounding_box.max.x()) {
      bounding_box.max.x() = mesh_bbox.max.x();
    }
    if (mesh_bbox.max.y() > bounding_box.max.y()) {
      bounding_box.max.y() = mesh_bbox.max.y();
    }
    if (mesh_bbox.max.z() > bounding_box.max.z()) {
      bounding_box.max.z() = mesh_bbox.max.z();
    }

    if (mesh_bbox.min.x() < bounding_box.min.x()) {
      bounding_box.min.x() = mesh_bbox.min.x();
    }
    if (mesh_bbox.min.y() < bounding_box.min.y()) {
      bounding_box.min.y() = mesh_bbox.min.y();
    }
    if (mesh_bbox.min.z() < bounding_box.min.z()) {
      bounding_box.min.z() = mesh_bbox.min.z();
    }

    int material_id = shapes[i].mesh.material_ids[0]; // TODO support per face materials.
    if (material_id == -1) {
      material_id = 0;
    }

    for (auto &material_buffer_list : material_buffers) {
      auto material_buffer = std::shared_ptr<UniformBuffer<Material>>(new VulkanEngine::UniformBuffer<Material>(2));
      Material material_data;
      material_data.ambient = {materials[material_id].ambient[0], materials[material_id].ambient[1], materials[material_id].ambient[2]};
      material_data.diffuse = {materials[material_id].diffuse[0], materials[material_id].diffuse[1], materials[material_id].diffuse[2]};
      material_data.specular = {materials[material_id].specular[0], materials[material_id].specular[1], materials[material_id].specular[2]};
      material_buffer->updateBuffer(&material_data, sizeof(material_data));
      material_buffer_list.push_back(material_buffer);
    }

    using RGBATexture2D1S =
        VulkanEngine::StagedBuffer<VulkanEngine::ShaderImage<
            vk::Format::eR8G8B8A8Unorm, vk::ImageType::e2D,
            vk::ImageTiling::eOptimal, vk::SampleCountFlagBits::e1>>;

    std::shared_ptr<RGBATexture2D1S> texture;

    if (materials[material_id].diffuse_texname != "") {
      int texture_width;
      int texture_height;
      int channels_in_file;
      unsigned char *image_data =
          stbi_load((std::string(mtl_path) + materials[material_id].diffuse_texname).c_str(),
                    &texture_width, &texture_height, &channels_in_file, 4);
      
      if (image_data) {
        texture.reset(new RGBATexture2D1S(
            vk::ImageLayout::eUndefined,
            vk::ImageUsageFlagBits::eTransferDst |
                vk::ImageUsageFlagBits::eTransferSrc |
                vk::ImageUsageFlagBits::eSampled,
            VMA_MEMORY_USAGE_GPU_ONLY, static_cast<uint32_t>(texture_width),
            static_cast<uint32_t>(texture_height), 1, sizeof(unsigned char) * 4,
            1,
            1, // TODO
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment));

        texture->setImageData(image_data);
        texture->createImageView(vk::ImageViewType::e2D,
                                 vk::ImageAspectFlagBits::eColor);
        texture->createSampler();
        texture->transferBuffer();
        textures.push_back(texture);

      } else {
        std::cerr << "OBJMesh texture: " << materials[i].diffuse_texname
                  << " could not be loaded" << std::endl;
      }
    }

    std::shared_ptr<Shader> shader;
    std::shared_ptr<ShaderModule> vertex_shader(
        new ShaderModule(getVertexShaderString(has_tex_coord, has_normals), false,
                        vk::ShaderStageFlagBits::eVertex));
    std::shared_ptr<ShaderModule> fragment_shader(
        new ShaderModule(getFragmentShaderString(texture.get() != nullptr, has_tex_coord, has_normals), false,
                        vk::ShaderStageFlagBits::eFragment));
    shader.reset(new Shader({fragment_shader, vertex_shader}));

    std::vector<std::vector<std::shared_ptr<Descriptor>>> descriptors;
    for (size_t j = 0; j < VulkanManager::getInstance().getFramesInFlight(); ++j) {
      std::vector<std::shared_ptr<Descriptor>> frame_descriptors;
      if (texture.get() != nullptr) {
        frame_descriptors.push_back(texture);
      }
      frame_descriptors.push_back(mvp_buffers[j]);
      frame_descriptors.push_back(material_buffers[j].back());
      descriptors.push_back(frame_descriptors);
    }

    shader->setDescriptors(descriptors);
    shaders.push_back(shader); 
  }

  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now() - begin)
                  .count();
  std::cout << "Mesh loading time: " << time << "(ms)" << std::endl;
}

const std::string
VulkanEngine::OBJMesh::getVertexShaderString(bool has_tex_coords, bool has_normals) const {
  std::stringstream return_string;

  return_string 
  << "#version 450\n"
  << "#extension GL_ARB_separate_shader_objects : enable\n"
  << "layout(binding = 0) uniform UniformBufferObject {\n"
  << "  mat4 model;\n"
  << "  mat4 view;\n"
  << "  mat4 proj;\n"
  << "} ubo;\n"
  << "layout(location = 0) in vec3 inPosition;\n";
  if (has_normals) {
    return_string
      << "layout(location = 1) in vec3 inNormal;\n"
      << "layout(location = 1) out vec3 outNormal;\n";
  }
  if (has_tex_coords) {
    return_string 
      << "layout(location = 2) in vec2 inTexcoords;\n"
      << "layout(location = 2) out vec2 outTexcoords;\n";
  }

  return_string 
    << "out gl_PerVertex {\n"
    << "  vec4 gl_Position;\n"
    << "};\n"
    << "void main() {\n"
    << "  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);\n";
  if (has_normals) {
    return_string
      << "  outNormal = mat3(transpose(inverse(ubo.model))) * inNormal;\n";
  }
  if (has_tex_coords) {
    return_string 
      << "  outTexcoords = inTexcoords;\n";
  }
  return_string 
    << "}\n";

  return return_string.str();
}

const std::string
VulkanEngine::OBJMesh::getFragmentShaderString(bool has_texture, bool has_tex_coords, bool has_normals) const {
  std::stringstream return_string;

  return_string 
    << "#version 450\n"
    << "#extension GL_ARB_separate_shader_objects : enable\n";

  if (has_normals) {
    return_string
      << "layout(location = 1) in vec3 inNormal;\n";
  }
  if (has_tex_coords) {
    return_string 
      << "layout(location = 2) in vec2 inTexcoords;\n";
  }

  return_string << "layout(location = 0) out vec4 outColor;\n";

  if (has_texture) {
    return_string 
      << "layout(binding = 1) uniform sampler2D texSampler;\n";
  }

  // TODO support materials
  return_string 
    << "layout(set = 0, binding = 2) uniform Material {\n"
    << "  vec3 ambient;\n"
    << "  vec3 diffuse;\n"
    << "  vec3 specular;\n"
    << "} material;\n";

  return_string 
    << "void main() {\n";
  if (has_tex_coords && has_texture) {
    return_string << "  vec4 texColor = texture(texSampler, inTexcoords);\n";
  } else {
    return_string << "  vec4 texColor = vec4(1.0);\n";
  }
  return_string
    << "  outColor = vec4(1.0) * texColor;\n"
    << "}\n";

  return return_string.str();
}
