// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include <memory>
#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

class Shader;
class MeshBase;

/// Defines a graphics pipeline. A graphics pipelines defines the rendering of a
/// mesh with a shader and rendering configurations of the object.
class GraphicsPipeline {
public:
  /// Contructor.
  GraphicsPipeline();

  /// Destructor.
  ~GraphicsPipeline();

  /// Bind the graphics pipeline to the current command buffer.
  void bindPipeline();

  /// Set the viewport for rendering. Defines how normalized device coordinates
  /// are converted to pixels in the framebuffer.
  /// \param x The x position of the viewport.
  /// \param y The y position of the viewport.
  /// \param width The width of the viewport.
  /// \param height The height of the viewport.
  /// \param min_depth The minimum depth to render.
  /// \param max_depth The maximum depth to render.
  void setViewPort(float x, float y, float width, float height, float min_depth,
                   float max_depth);

  /// Specify a scissor, i.e. the area in the viewport that is rendered to.
  /// \param offset_x The x offset that defines position of the scissor.
  /// \param offset_y The y offset that defines the position of the scissor.
  /// \param width The width of the scissor along x.
  /// \param height The height of the scissor along y.
  void setScissor(int32_t offset_x, int32_t offset_y, int32_t width,
                  int32_t height);

  /// Create the graphics pipeline.
  /// \param mesh The mesh to render.
  /// \param shader The shader to render the mesh with.
  void createGraphicsPipeline(const std::shared_ptr<MeshBase> mesh,
                              const std::shared_ptr<Shader> shader);

private:
  /// Internal vulkan instance of the graphics pipeline.
  vk::Pipeline vk_graphics_pipeline;

  /// Internal vulkan instance of the viewport.
  vk::Viewport vk_viewport;

  /// Internal vulkan instance of the scissor.
  vk::Rect2D vk_scissor;
};

} // namespace VulkanEngine

#endif /* GRAPHICSPIPELINE_H */
