#include <VulkanTest/Window.h>

VulkanTest::Window::Window( int _width, int _height, const std::string& _title, bool _full_screen ) 
  : width( _width ), height( _height ), title( _title ), full_screen( _full_screen ) {
}

VulkanTest::Window::~Window() {
}

int VulkanTest::Window::getWidth() { 
  return width;
}

int VulkanTest::Window::getHeight() { 
  return height; 
}

const std::string& VulkanTest::Window::getTitle() {
  return title;
}

bool VulkanTest::Window::isFullScreen() {
  return full_screen;
}