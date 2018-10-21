#include <VulkanEngine/Window.h>

VulkanEngine::Window::Window( uint32_t _width, uint32_t _height, const std::string& _title, bool _full_screen ) 
  : width( _width ), height( _height ), title( _title ), full_screen( _full_screen ), size_changed( false ) {
}

VulkanEngine::Window::~Window() {
}

std::shared_ptr< VulkanEngine::MouseInput > VulkanEngine::Window::getMouseInput() {
  return mouse_input;
}

void VulkanEngine::Window::setWidth( uint32_t _width ) {
  width = _width;
}

void VulkanEngine::Window::setHeight( uint32_t _height ) {
  height = _height;
}

uint32_t VulkanEngine::Window::getWidth() { 
  return width;
}

uint32_t VulkanEngine::Window::getHeight() { 
  return height; 
}

uint32_t VulkanEngine::Window::getFramebufferWidth() {
  return framebuffer_width;
}

uint32_t VulkanEngine::Window::getFramebufferHeight() {
  return framebuffer_height;
}

const std::string& VulkanEngine::Window::getTitle() {
  return title;
}

bool VulkanEngine::Window::isFullScreen() {
  return full_screen;
}

bool VulkanEngine::Window::sizeHasChanged() {
  bool tmp_size_changed = size_changed;
  size_changed = false;
  return tmp_size_changed;
}

void VulkanEngine::Window::mousePositionCallback( double xpos, double ypos ) {

  if( mouse_input.get() ){
    mouse_input->setPosition( xpos, ypos );
  }

}

void VulkanEngine::Window::mouseButtonPressedCallback( bool left_pressed, bool right_pressed, bool middle_pressed ) {

  if( mouse_input.get() ){
    mouse_input->setButtons( left_pressed, right_pressed, middle_pressed );
  }

}

void keyboardButtonPressedCallback( const char* key, int scancode ) {

}