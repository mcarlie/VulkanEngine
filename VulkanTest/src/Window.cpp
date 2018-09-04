#include <VulkanTest/Window.h>

VulkanTest::Window::Window( uint32_t _width, uint32_t _height, const std::string& _title, bool _full_screen ) 
  : width( _width ), height( _height ), title( _title ), full_screen( _full_screen ) {
}

VulkanTest::Window::~Window() {
}

uint32_t VulkanTest::Window::getWidth() { 
  return width;
}

uint32_t VulkanTest::Window::getHeight() { 
  return height; 
}

const std::string& VulkanTest::Window::getTitle() {
  return title;
}

bool VulkanTest::Window::isFullScreen() {
  return full_screen;
}