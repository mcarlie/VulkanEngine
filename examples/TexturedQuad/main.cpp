// Copyright (c) 2026 Michael Carlie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <VulkanEngine/Camera.h>
#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/GraphicsPipeline.h>
#include <VulkanEngine/IndexAttribute.h>
#include <VulkanEngine/Mesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/Shader.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/ShaderModule.h>
#include <VulkanEngine/SingleUsageCommandBuffer.h>
#include <VulkanEngine/StagedBuffer.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/VertexAttribute.h>
#include <VulkanEngine/VulkanManager.h>

#include <Eigen/Eigen>
#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace {

struct MvpUbo {
  Eigen::Matrix4f model;
  Eigen::Matrix4f view;
  Eigen::Matrix4f projection;
};

using TexturedQuadMesh =
    VulkanEngine::Mesh<Eigen::Vector3f, uint32_t, Eigen::Vector2f>;

using RGBATexture2D = VulkanEngine::StagedBuffer<VulkanEngine::ShaderImage<
    vk::Format::eR8G8B8A8Unorm, vk::ImageType::e2D, vk::ImageTiling::eOptimal,
    vk::SampleCountFlagBits::e1>>;

void moveCamera(std::shared_ptr<VulkanEngine::KeyboardInput> keyboard_input,
                std::shared_ptr<VulkanEngine::Camera> camera,
                float speed = 0.03) {
  // Same controls as examples/SimpleScene:
  // - W/S: forward/back
  // - A/D: left/right
  // - Z/X: up/down
  const auto key_w = keyboard_input->getLastKeyStatus(GLFW_KEY_W);
  const auto key_a = keyboard_input->getLastKeyStatus(GLFW_KEY_A);
  const auto key_s = keyboard_input->getLastKeyStatus(GLFW_KEY_S);
  const auto key_d = keyboard_input->getLastKeyStatus(GLFW_KEY_D);
  const auto key_z = keyboard_input->getLastKeyStatus(GLFW_KEY_Z);
  const auto key_x = keyboard_input->getLastKeyStatus(GLFW_KEY_X);

  Eigen::Vector3f camera_movement = {0.0f, 0.0f, 0.0f};

  if (key_a == VulkanEngine::KeyboardInput::PRESSED ||
      key_a == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(0) -= speed;
  }
  if (key_d == VulkanEngine::KeyboardInput::PRESSED ||
      key_d == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(0) += speed;
  }
  if (key_z == VulkanEngine::KeyboardInput::PRESSED ||
      key_z == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(1) -= speed;
  }
  if (key_x == VulkanEngine::KeyboardInput::PRESSED ||
      key_x == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(1) += speed;
  }
  if (key_w == VulkanEngine::KeyboardInput::PRESSED ||
      key_w == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(2) -= speed;
  }
  if (key_s == VulkanEngine::KeyboardInput::PRESSED ||
      key_s == VulkanEngine::KeyboardInput::REPEAT) {
    camera_movement(2) += speed;
  }

  camera->setLookAt(camera->getLookAt() + camera_movement);
  camera->setTransform(Eigen::Affine3f(Eigen::Translation3f(camera_movement) *
                                       Eigen::Affine3f(camera->getTransform()))
                           .matrix());
}

std::string getVertexShaderString() {
  return std::string{
      "#version 450\n"
      "#extension GL_ARB_separate_shader_objects : enable\n"
      "\n"
      "layout(binding = 0) uniform UniformBufferObject {\n"
      "  mat4 model;\n"
      "  mat4 view;\n"
      "  mat4 proj;\n"
      "} ubo;\n"
      "\n"
      "layout(location = 0) in vec3 inPosition;\n"
      "layout(location = 1) in vec2 inTexCoord;\n"
      "\n"
      "layout(location = 0) out vec2 fragTexCoord;\n"
      "\n"
      "out gl_PerVertex {\n"
      "  vec4 gl_Position;\n"
      "};\n"
      "\n"
      "void main() {\n"
      "  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, "
      "1.0);\n"
      "  fragTexCoord = inTexCoord;\n"
      "}\n"};
}

std::string getFragmentShaderString() {
  return std::string{
      "#version 450\n"
      "#extension GL_ARB_separate_shader_objects : enable\n"
      "\n"
      "layout(binding = 1) uniform sampler2D texSampler;\n"
      "\n"
      "layout(location = 0) in vec2 fragTexCoord;\n"
      "\n"
      "layout(location = 0) out vec4 outFragColor;\n"
      "\n"
      "void main() {\n"
      "  outFragColor = texture(texSampler, fragTexCoord);\n"
      "}\n"};
}

class TexturedQuadObject : public VulkanEngine::SceneObject {
 public:
  TexturedQuadObject() = default;

  void initialize() {
    auto& vulkan_manager = VulkanEngine::VulkanManager::getInstance();

    // Quad vertices: positions and UVs
    const std::array<Eigen::Vector3f, 4> positions = {
        Eigen::Vector3f{-0.5f, -0.5f, 0.0f},  // bottom-left
        Eigen::Vector3f{0.5f, -0.5f, 0.0f},   // bottom-right
        Eigen::Vector3f{0.5f, 0.5f, 0.0f},    // top-right
        Eigen::Vector3f{-0.5f, 0.5f, 0.0f},   // top-left
    };
    const std::array<Eigen::Vector2f, 4> uvs = {
        Eigen::Vector2f{0.0f, 0.0f},  // bottom-left
        Eigen::Vector2f{1.0f, 0.0f},  // bottom-right
        Eigen::Vector2f{1.0f, 1.0f},  // top-right
        Eigen::Vector2f{0.0f, 1.0f},  // top-left
    };
    const std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

    auto position_attribute =
        std::make_shared<VulkanEngine::VertexAttribute<Eigen::Vector3f>>(
            positions.data(), positions.size(), 0,
            vk::Format::eR32G32B32Sfloat);
    auto index_attribute =
        std::make_shared<VulkanEngine::IndexAttribute<uint32_t>>(
            indices.data(), indices.size());

    auto uv_attribute =
        std::make_shared<VulkanEngine::VertexAttribute<Eigen::Vector2f>>(
            uvs.data(), uvs.size(), 1, vk::Format::eR32G32Sfloat);

    using UVContainer =
        typename TexturedQuadMesh::template AttributeContainer<Eigen::Vector2f>;
    UVContainer uv_container;
    uv_container.emplace_back(uv_attribute);

    std::tuple<UVContainer> additional_attributes(uv_container);

    mesh = std::make_shared<TexturedQuadMesh>();
    mesh->setPositions(position_attribute);
    mesh->setIndices(index_attribute);
    mesh->setAttributes(additional_attributes);

    // Bounding box
    Eigen::Vector3f max_pos = positions[0];
    Eigen::Vector3f min_pos = positions[0];
    for (const auto& p : positions) {
      max_pos = max_pos.cwiseMax(p);
      min_pos = min_pos.cwiseMin(p);
    }
    mesh->setBoundingBox(max_pos, min_pos);

    // Transfer mesh to GPU
    VulkanEngine::SingleUsageCommandBuffer command_buffer;
    command_buffer.beginSingleUsageCommandBuffer();
    mesh->transferBuffers(command_buffer.single_use_command_buffer);
    command_buffer.endSingleUsageCommandBuffer();

    // Create MVP uniform buffers
    mvp_buffers.resize(vulkan_manager.getFramesInFlight());
    for (auto& ub : mvp_buffers) {
      ub = std::make_shared<VulkanEngine::UniformBuffer<MvpUbo>>(0);
    }

    // Create texture: simple checkerboard pattern
    const uint32_t tex_width = 256;
    const uint32_t tex_height = 256;
    const uint32_t tex_channels = 4;  // RGBA
    const size_t pixel_size = tex_channels * sizeof(uint8_t);
    std::vector<uint8_t> texture_data(tex_width * tex_height * tex_channels);

    for (uint32_t y = 0; y < tex_height; ++y) {
      for (uint32_t x = 0; x < tex_width; ++x) {
        uint32_t index = (y * tex_width + x) * tex_channels;
        bool is_white = ((x / 32) + (y / 32)) % 2 == 0;
        texture_data[index + 0] = is_white ? 255 : 0;  // R
        texture_data[index + 1] = is_white ? 255 : 0;  // G
        texture_data[index + 2] = is_white ? 255 : 0;  // B
        texture_data[index + 3] = 255;                 // A
      }
    }

    texture = std::make_shared<RGBATexture2D>(
        vk::ImageLayout::eUndefined,
        vk::ImageUsageFlagBits::eTransferDst |
            vk::ImageUsageFlagBits::eTransferSrc |
            vk::ImageUsageFlagBits::eSampled,
        VMA_MEMORY_USAGE_GPU_ONLY, tex_width, tex_height, 1, pixel_size, 1, 1,
        vk::DescriptorType::eCombinedImageSampler,
        vk::ShaderStageFlagBits::eFragment);

    // Upload texture data
    texture->setImageData(texture_data.data());

    // Ensure proper image view and sampler
    texture->createImageView(vk::ImageViewType::e2D,
                             vk::ImageAspectFlagBits::eColor);
    texture->createSampler();

    // Transfer the texture buffer to GPU
    VulkanEngine::SingleUsageCommandBuffer tex_cmd_buffer;
    tex_cmd_buffer.beginSingleUsageCommandBuffer();
    texture->transferBuffer(tex_cmd_buffer.single_use_command_buffer);
    tex_cmd_buffer.endSingleUsageCommandBuffer();

    // Create shader modules
    auto vertex_shader = std::make_shared<VulkanEngine::ShaderModule>(
        getVertexShaderString(), false, vk::ShaderStageFlagBits::eVertex);
    auto fragment_shader = std::make_shared<VulkanEngine::ShaderModule>(
        getFragmentShaderString(), false, vk::ShaderStageFlagBits::eFragment);
    shader = std::make_shared<VulkanEngine::Shader>(
        std::vector<std::shared_ptr<VulkanEngine::ShaderModule>>{
            fragment_shader, vertex_shader});

    // Create per-frame descriptor sets
    std::vector<std::vector<std::shared_ptr<VulkanEngine::Descriptor>>>
        descriptors;
    descriptors.reserve(vulkan_manager.getFramesInFlight());
    for (size_t j = 0; j < vulkan_manager.getFramesInFlight(); ++j) {
      std::vector<std::shared_ptr<VulkanEngine::Descriptor>> frame_desc;
      frame_desc.push_back(mvp_buffers[j]);
      frame_desc.push_back(texture);
      descriptors.push_back(frame_desc);
    }
    shader->setDescriptors(descriptors);
  }

 private:
  void update(std::shared_ptr<VulkanEngine::SceneState> scene_state) override {
    MvpUbo ubo_data;
    ubo_data.projection = scene_state->getProjectionMatrix();
    ubo_data.view = scene_state->getViewMatrix();
    ubo_data.model = scene_state->getTotalTransform();

    for (auto& ub : mvp_buffers) {
      ub->updateBuffer(&ubo_data, sizeof(ubo_data));
    }

    auto& vulkan_manager = VulkanEngine::VulkanManager::getInstance();
    const auto window = scene_state->getScene().getActiveWindow();

    if (graphics_pipeline_updated) {
      graphics_pipeline_updated = !window->sizeHasChanged();
    }

    if (!graphics_pipeline_updated) {
      const int32_t width = static_cast<int32_t>(window->getFramebufferWidth());
      const int32_t height =
          static_cast<int32_t>(window->getFramebufferHeight());

      graphics_pipeline = std::make_shared<VulkanEngine::GraphicsPipeline>();
      graphics_pipeline->setViewPort(0, 0, static_cast<float>(width),
                                     static_cast<float>(height), 0.0f, 1.0f);
      graphics_pipeline->setScissor(0, 0, width, height);
      graphics_pipeline->setCullMode(vk::CullModeFlagBits::eNone);
      graphics_pipeline->createGraphicsPipeline(mesh, shader);
    }

    graphics_pipeline->bindPipeline();

    auto current_command_buffer = vulkan_manager.getCurrentCommandBuffer();
    mesh->bindVertexBuffers(current_command_buffer);
    mesh->bindIndexBuffer(current_command_buffer);
    shader->bindDescriptorSet(
        current_command_buffer,
        static_cast<uint32_t>(vulkan_manager.getCurrentFrame()));
    mesh->draw(current_command_buffer);

    graphics_pipeline_updated = true;
    VulkanEngine::SceneObject::update(scene_state);
  }

 private:
  std::shared_ptr<TexturedQuadMesh> mesh;
  std::shared_ptr<VulkanEngine::Shader> shader;
  std::shared_ptr<VulkanEngine::GraphicsPipeline> graphics_pipeline;
  std::shared_ptr<RGBATexture2D> texture;

  std::vector<std::shared_ptr<VulkanEngine::UniformBuffer<MvpUbo>>> mvp_buffers;
  bool graphics_pipeline_updated = false;
};

}  // namespace

int main() {
  const uint32_t window_width = 1280;
  const uint32_t window_height = 800;
  const std::string title = "TexturedQuad";

  auto window = std::make_shared<VulkanEngine::GLFWWindow>(
      window_width, window_height, title, false);
  if (!window->initialize()) {
    return 1;
  }

  auto& vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  if (!vulkan_manager.initialize(window)) {
    return 1;
  }

  auto windows = std::vector<std::shared_ptr<VulkanEngine::Window>>{window};
  auto scene = std::make_shared<VulkanEngine::Scene>(windows);

  auto camera = std::make_shared<VulkanEngine::Camera>(
      Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 1.0f, 0.0f),
      0.1f, 10.0f, 45.0f, window->getFramebufferWidth(),
      window->getFramebufferHeight());
  camera->setTransform(
      Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, 3.0f)).matrix());

  auto textured_quad = std::make_shared<TexturedQuadObject>();
  textured_quad->initialize();

  // Important: camera must come before the quad so view/projection are
  // updated in the scene_state before we build MVP for rendering.
  scene->addChildren({camera, textured_quad});

  const auto keyboard_input = window->getKeyboardInput();
  Eigen::Vector2d prev_mouse_position = {0.0, 0.0};
  while (!window->shouldClose()) {
    if (keyboard_input.get()) {
      const auto& mouse_input = window->getMouseInput();
      const auto mouse_position = mouse_input->getPosition();

      if (mouse_input->leftButtonPressed()) {
        auto mouse_movement = -(mouse_position - prev_mouse_position) * 0.01;

        auto rotation_x =
            Eigen::AngleAxisf(mouse_movement.x(), Eigen::Vector3f::UnitY());
        auto rotation_y =
            Eigen::AngleAxisf(mouse_movement.y(), Eigen::Vector3f::UnitX());
        auto rotation = rotation_x * rotation_y;

        // Rotate quad in local space
        Eigen::Transform<float, 3, Eigen::Affine> transform(
            textured_quad->getTransform());
        transform = transform.inverse();
        transform.rotate(rotation);
        transform = transform.inverse();
        textured_quad->setTransform(transform.matrix());

        const auto scroll_offset_2d = mouse_input->getScrollOffset();
        if (scroll_offset_2d.y() != 0.0) {
          Eigen::Vector3f position = camera->getTransform().block<3, 1>(0, 3);
          Eigen::Vector3f look_at = camera->getLookAt();
          Eigen::Vector3f direction = (look_at - position).normalized();
          float zoom_speed = 0.1f;
          float zoom_amount = -scroll_offset_2d.y() * zoom_speed;
          Eigen::Vector3f new_position = position + direction * zoom_amount;
          camera->setTransform(
              Eigen::Affine3f(Eigen::Translation3f(new_position)).matrix());
        }

        moveCamera(keyboard_input, camera);
      }

      prev_mouse_position = mouse_position;
    }

    scene->update();
    vulkan_manager.drawImage();
  }

  return 0;
}
