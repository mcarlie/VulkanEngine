#ifndef KEYBOARDINPUT_H
#define KEYBOARDINPUT_H

#include <unordered_map>

namespace VulkanEngine {

  class KeyboardInput {

    friend class Window;

  public:

    enum KeyStatus {
      PRESSED,
      RELEASED,
      REPEAT
    };

    KeyboardInput();
    
    ~KeyboardInput();

  private:

    void setKeyStatus( int scan_code );

    std::unordered_map< std::string, KeyStatus > key_status;

  };

}

#endif /* KEYBOARDINPUT_H */