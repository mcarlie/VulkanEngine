#include <VulkanEngine/KeyboardInput.h>

const VulkanEngine::KeyboardInput::KeyStatus
VulkanEngine::KeyboardInput::getLastKeyStatus(int key_id) {
  for (const auto val : key_status_map) {
    if (val.second.key_id == key_id) {
      return val.second.status;
    }
  }
  return NO_STATUS;
}
