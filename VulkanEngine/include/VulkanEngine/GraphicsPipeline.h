#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include <memory>
#include <vulkan/vulkan.hpp>


namespace VulkanEngine {

class Shader;
class MeshBase;

class GraphicsPipeline {

public:
  GraphicsPipeline();

  ~GraphicsPipeline();

  void bindPipeline();

  void setViewPort(float x, float y, float width, float height, float min_depth,
                   float min_height);

  void setScissor(int32_t offset_x, int32_t offset_y, int32_t width_x,
                  int32_t width_y);

protected:
  void createGraphicsPipeline(const std::shared_ptr<MeshBase> &mesh,
                              const std::shared_ptr<Shader> &shader);

private:
  vk::Pipeline vk_graphics_pipeline;

  vk::Viewport vk_viewport;

  vk::Rect2D vk_scissor;
};

} // namespace VulkanEngine

#endif /* GRAPHICSPIPELINE_H */
