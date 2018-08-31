#include <VulkanTest/glfw_window.h>

#include <iostream>

VulkanTest::GLFW_Window::GLFW_Window( int _width, int _height, const std::string& _title, bool _full_screen ) 
  : Window( _width, _height, _title, _full_screen ) {
  glfwSetErrorCallback( errorCallback );
}

VulkanTest::GLFW_Window::~GLFW_Window() {
  glfwTerminate();
}

bool VulkanTest::GLFW_Window::initialize() {

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
    full_screen ? glfwGetPrimaryMonitor() : nullptr,
    nullptr );
  
  if( !glfw_window ) {
    return false;
  }

  //VkResult result = glfwCreateWindowSurface(  )

  return true;

}

bool VulkanTest::GLFW_Window::createVulkanSurface( const VkInstance& instance ) {

  VkResult err = glfwCreateWindowSurface( instance, glfw_window, nullptr, &vulkan_surface );
  if( err ) {
    // TODO
  }

}

void VulkanTest::GLFW_Window::errorCallback( int error, const char* description ) {
  std::cerr << "GLFW error: " << description << " error code:" << error << std::endl;
}