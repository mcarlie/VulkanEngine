#ifndef SCENE_H
#define SCENE_H

#include <VulkanEngine/SceneObject.h>
#include <VulkanEngine/Window.h>

#include <memory>

namespace VulkanEngine {

class Scene : public SceneObject {
public:
  /// Constructor.
  Scene(const std::vector<std::shared_ptr<Window>> &_windows);

  /// Destructor.
  virtual ~Scene();

  void update();

  const std::shared_ptr<Window> &getActiveWindow() const;

private:
  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void update(SceneState &scene_state) override;

  std::unique_ptr<SceneState> state_instance;

  std::vector<std::shared_ptr<Window>> windows;
};

} // namespace VulkanEngine

#endif /* SCENE_H */
