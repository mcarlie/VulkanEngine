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

#include <ExampleUtils.h>
#include <VulkanEngine/Camera.h>
#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/GraphicsPipeline.h>
#include <VulkanEngine/IndexAttribute.h>
#include <VulkanEngine/Mesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/Shader.h>
#include <VulkanEngine/ShaderModule.h>
#include <VulkanEngine/SingleUsageCommandBuffer.h>
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

using TriangleMesh =
    VulkanEngine::Mesh<Eigen::Vector3f, uint32_t, Eigen::Vector3f>;

std::string getVertexShaderString() {
  // Minimal vertex shader:
  // - Uses the engine's MVP uniform buffer (binding = 0)
  // - Passes per-vertex color through a location-matched output
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
      "layout(location = 1) in vec3 inColor;\n"
      "\n"
      "layout(location = 0) out vec3 outColor;\n"
      "\n"
      "out gl_PerVertex {\n"
      "  vec4 gl_Position;\n"
      "};\n"
      "\n"
      "void main() {\n"
      "  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, "
      "1.0);\n"
      "  outColor = inColor;\n"
      "}\n"};
}

std::string getFragmentShaderString() {
  return std::string{
      "#version 450\n"
      "#extension GL_ARB_separate_shader_objects : enable\n"
      "\n"
      "layout(location = 0) in vec3 inColor;\n"
      "\n"
      "layout(location = 0) out vec4 outFragColor;\n"
      "\n"
      "void main() {\n"
      "  outFragColor = vec4(inColor, 1.0);\n"
      "}\n"};
}

class TriangleObject : public VulkanEngine::SceneObject {
 public:
  TriangleObject() = default;

  void initialize() {
    auto& vulkan_manager = VulkanEngine::VulkanManager::getInstance();

    // CCW (and safe for backface culling with the engine's projection).
    // If you don't see a triangle, swap the index order: {0,2,1}.
    const std::array<Eigen::Vector3f, 3> positions = {
        Eigen::Vector3f{-0.5f, -0.5f, 0.0f},
        Eigen::Vector3f{0.5f, -0.5f, 0.0f},
        Eigen::Vector3f{0.0f, 0.5f, 0.0f},
    };
    const std::array<Eigen::Vector3f, 3> colors = {
        Eigen::Vector3f{1.0f, 0.0f, 0.0f},  // red
        Eigen::Vector3f{0.0f, 1.0f, 0.0f},  // green
        Eigen::Vector3f{0.0f, 0.0f, 1.0f},  // blue
    };
    const std::array<uint32_t, 3> indices = {0u, 1u, 2u};

    auto position_attribute =
        std::make_shared<VulkanEngine::VertexAttribute<Eigen::Vector3f>>(
            positions.data(), positions.size(), 0,
            vk::Format::eR32G32B32Sfloat);
    auto index_attribute =
        std::make_shared<VulkanEngine::IndexAttribute<uint32_t>>(
            indices.data(), indices.size());

    auto color_attribute =
        std::make_shared<VulkanEngine::VertexAttribute<Eigen::Vector3f>>(
            colors.data(), colors.size(), 1, vk::Format::eR32G32B32Sfloat);

    using ColorContainer =
        typename TriangleMesh::template AttributeContainer<Eigen::Vector3f>;
    ColorContainer color_container;
    color_container.emplace_back(color_attribute);

    std::tuple<ColorContainer> additional_attributes(color_container);

    mesh = std::make_shared<TriangleMesh>();
    mesh->setPositions(position_attribute);
    mesh->setIndices(index_attribute);
    mesh->setAttributes(additional_attributes);

    // Bounding box used by the mesh abstraction.
    Eigen::Vector3f max_pos = positions[0];
    Eigen::Vector3f min_pos = positions[0];
    for (const auto& p : positions) {
      max_pos = max_pos.cwiseMax(p);
      min_pos = min_pos.cwiseMin(p);
    }
    mesh->setBoundingBox(max_pos, min_pos);

    // Transfer the mesh vertex/index data to GPU memory.
    VulkanEngine::SingleUsageCommandBuffer command_buffer;
    command_buffer.beginSingleUsageCommandBuffer();
    mesh->transferBuffers(command_buffer.single_use_command_buffer);
    command_buffer.endSingleUsageCommandBuffer();

    // Create MVP uniform buffers for frames in flight.
    mvp_buffers.resize(vulkan_manager.getFramesInFlight());
    for (auto& ub : mvp_buffers) {
      ub = std::make_shared<VulkanEngine::UniformBuffer<MvpUbo>>(0);
    }

    // Create shader modules from GLSL strings.
    auto vertex_shader = std::make_shared<VulkanEngine::ShaderModule>(
        getVertexShaderString(), false, vk::ShaderStageFlagBits::eVertex);
    auto fragment_shader = std::make_shared<VulkanEngine::ShaderModule>(
        getFragmentShaderString(), false, vk::ShaderStageFlagBits::eFragment);
    shader = std::make_shared<VulkanEngine::Shader>(
        std::vector<std::shared_ptr<VulkanEngine::ShaderModule>>{
            fragment_shader, vertex_shader});

    // Create per-frame descriptor sets (same layout, different buffer
    // instances).
    std::vector<std::vector<std::shared_ptr<VulkanEngine::Descriptor>>>
        descriptors;
    descriptors.reserve(vulkan_manager.getFramesInFlight());
    for (size_t j = 0; j < vulkan_manager.getFramesInFlight(); ++j) {
      std::vector<std::shared_ptr<VulkanEngine::Descriptor>> frame_desc;
      frame_desc.push_back(mvp_buffers[j]);
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
      // Ensure we can see the triangle regardless of winding direction.
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
  std::shared_ptr<TriangleMesh> mesh;
  std::shared_ptr<VulkanEngine::Shader> shader;
  std::shared_ptr<VulkanEngine::GraphicsPipeline> graphics_pipeline;

  std::vector<std::shared_ptr<VulkanEngine::UniformBuffer<MvpUbo>>> mvp_buffers;
  bool graphics_pipeline_updated = false;
};

}  // namespace

int main() {
  const uint32_t window_width = 1280;
  const uint32_t window_height = 800;
  const std::string title = "Triangle (gradient)";

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

  auto triangle = std::make_shared<TriangleObject>();
  triangle->initialize();

  // Important: camera must come before the triangle so view/projection are
  // updated in the scene_state before we build MVP for rendering.
  scene->addChildren({camera, triangle});

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

        // Rotate triangle in local space using the same approach as
        // examples/SimpleScene.
        Eigen::Transform<float, 3, Eigen::Affine> transform(
            triangle->getTransform());
        transform = transform.inverse();
        transform.rotate(rotation);
        transform = transform.inverse();
        triangle->setTransform(transform.matrix());
      }

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

      prev_mouse_position = mouse_position;
    }

    scene->update();
    vulkan_manager.drawImage();
  }

  return 0;
}
