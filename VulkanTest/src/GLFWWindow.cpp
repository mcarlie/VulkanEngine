#include <VulkanTest/GLFWWindow.h>

#include <iostream>

VulkanTest::GLFWWindow::GLFWWindow( int _width, int _height, const std::string& _title, bool _full_screen ) 
  : Window( _width, _height, _title, _full_screen ) {
  glfwSetErrorCallback( errorCallback );
}

VulkanTest::GLFWWindow::~GLFWWindow() {
  glfwDestroyWindow( glfw_window );
  glfwTerminate();
}

void VulkanTest::GLFWWindow::update() {
  glfwPollEvents();
}

bool VulkanTest::GLFWWindow::initialize() {

  if( !glfwInit() ) {
    return false;
  }

  if( !glfwVulkanSupported() ) {
    std::cerr 
      << "Failed to initialize window. "
      << "Vulkan not supported by GLFW" << std::endl;
    return false;
  }

  glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
  glfw_window = glfwCreateWindow( 
    getWidth(),
    getHeight(),
    getTitle().c_str(),
    isFullScreen() ? glfwGetPrimaryMonitor() : nullptr,
    nullptr );
  
  if( !glfw_window ) {
    return false;
  }

  return true;

}

vk::SurfaceKHR VulkanTest::GLFWWindow::createVulkanSurface( const vk::Instance& instance ) {

  VkSurfaceKHR surface;
  VkResult err = glfwCreateWindowSurface( instance, glfw_window, nullptr, &surface );
  if( err != VK_SUCCESS ) {
    throw std::runtime_error( "Could not create Vulkan surface" );
  }

  return surface;

}

const std::vector< const char* > VulkanTest::GLFWWindow::getRequiredVulkanInstanceExtensions() {
  uint32_t num_extensions;
  const char** extension_names;
  extension_names = glfwGetRequiredInstanceExtensions( &num_extensions );
  std::vector< const char* > extensions; 
  for( uint32_t i = 0; i < num_extensions; ++i ) {
    extensions.push_back( extension_names[i] );
  }
  return extensions;
}

bool VulkanTest::GLFWWindow::shouldClose() {
  return glfwWindowShouldClose( glfw_window );
}

void VulkanTest::GLFWWindow::errorCallback( int error, const char* description ) {
  std::cerr << "GLFW error: " << description << " error code:" << error << std::endl;
}