// Copyright (c) 2025 Michael Carlie
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

#include <VulkanEngine/RenderPass.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/VulkanManager.h>

#include <memory>
#include <vector>

VulkanEngine::Scene::Scene(const std::vector<std::shared_ptr<Window>>& _windows)
    : windows(_windows) {}

VulkanEngine::Scene::~Scene() {}

void VulkanEngine::Scene::update() {
  if (!state_instance.get()) {
    state_instance.reset(new SceneState(*this));
  }

  for (const auto window : windows) {
    if (window.get() != nullptr) {
      window->update();
    }
  }
  VulkanManager::getInstance().getDefaultRenderPass()->begin();
  SceneObject::update(state_instance);
  VulkanManager::getInstance().getDefaultRenderPass()->end();
}

const std::shared_ptr<VulkanEngine::Window>
VulkanEngine::Scene::getActiveWindow() const {
  for (size_t i = 0; i < windows.size(); ++i) {
    return windows[i];
  }

  return std::shared_ptr<VulkanEngine::Window>();
}

void VulkanEngine::Scene::update(std::shared_ptr<SceneState> scene_state) {}
