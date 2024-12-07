// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef SCENE_H
#define SCENE_H

#include <VulkanEngine/SceneObject.h>
#include <VulkanEngine/Window.h>

#include <memory>

namespace VulkanEngine {

class Scene : public SceneObject {
 public:
  /// Constructor.
  Scene(const std::vector<std::shared_ptr<Window>>& _windows);

  /// Destructor.
  virtual ~Scene();

  /// Update the scene object.
  void update();

  /// Get the currently active window.
  const std::shared_ptr<Window> getActiveWindow() const;

 private:
  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void update(SceneState& scene_state) override;

  /// The current state of the scene.
  std::unique_ptr<SceneState> state_instance;

  /// List of windows to render to.
  std::vector<std::shared_ptr<Window>> windows;
};

}  // namespace VulkanEngine

#endif /* SCENE_H */
