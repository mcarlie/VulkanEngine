// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_KEYBOARDINPUT_H_
#define INCLUDE_VULKANENGINE_KEYBOARDINPUT_H_

#include <map>

namespace VulkanEngine {

/// Class which provides ability to read keyboard input from windowing system.
class KeyboardInput {
  friend class Window;

 public:
  /// Represents different states that keys can be in.
  enum KeyStatus { PRESSED, RELEASED, REPEAT, NO_STATUS };

  /// Contains information about a particular key.
  struct KeyInfo {
    /// The current status of the key.
    KeyStatus status;

    /// The windowing system specific key id.
    int key_id;
  };

  /// Get the last status of the key provided.
  /// \param key_id The windowing system specific key id.
  /// \return The status of the provided key.
  const KeyStatus getLastKeyStatus(int key_id);

 private:
  /// Maps key scancodes to KeyInfo instances.
  std::map<int, KeyInfo> key_status_map;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_KEYBOARDINPUT_H_
