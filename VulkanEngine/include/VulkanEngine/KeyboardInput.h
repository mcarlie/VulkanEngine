#ifndef KEYBOARDINPUT_H
#define KEYBOARDINPUT_H

#include <map>

namespace VulkanEngine {

  class KeyboardInput {

    friend class Window;

  public:

    enum KeyStatus {
      PRESSED,
      RELEASED,
      REPEAT,
      NO_STATUS
    };
    
    struct KeyInfo {
      KeyStatus status;
      int key_id;
    };
    
    const KeyStatus getLastKeyStatus( int key_id );

  private:

    std::map< int, KeyInfo > key_status_map;

  };

}

#endif /* KEYBOARDINPUT_H */
