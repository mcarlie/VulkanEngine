#include <VulkanTest/Window.h>

VulkanTest::Window::Window( uint32_t _width, uint32_t _height, const std::string& _title, bool _full_screen ) 
  : width( _width ), height( _height ), title( _title ), full_screen( _full_screen ) {
}

VulkanTest::Window::~Window() {
}

void VulkanTest::Window::setWidth( uint32_t _width ) {
  width = _width;
}

void VulkanTest::Window::setHeight( uint32_t _height ) {
  height = _height;
}

uint32_t VulkanTest::Window::getWidth() { 
  return width;
}

uint32_t VulkanTest::Window::getHeight() { 
  return height; 
}

uint32_t VulkanTest::Window::getFramebufferWidth() {
  return framebuffer_width;
}

uint32_t VulkanTest::Window::getFramebufferHeight() {
  return framebuffer_height;
}

const std::string& VulkanTest::Window::getTitle() {
  return title;
}

bool VulkanTest::Window::isFullScreen() {
  return full_screen;
}