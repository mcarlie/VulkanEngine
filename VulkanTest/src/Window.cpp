#include <VulkanEngine/Window.h>

VulkanEngine::Window::Window( uint32_t _width, uint32_t _height, const std::string& _title, bool _full_screen ) 
  : width( _width ), height( _height ), title( _title ), full_screen( _full_screen ), size_changed( false ) {
}

VulkanEngine::Window::~Window() {
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