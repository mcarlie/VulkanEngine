#include <VulkanEngine/GLFWWindow.h>

#include <iostream>

VulkanEngine::GLFWWindow::GLFWWindow( int _width, int _height, const std::string& _title, bool _full_screen ) 
  : glfw_window( nullptr ), Window( _width, _height, _title, _full_screen ) {

  glfwSetErrorCallback( errorCallback );

}

VulkanEngine::GLFWWindow::~GLFWWindow() {

  glfwDestroyWindow( glfw_window );
  glfwTerminate();

}

void VulkanEngine::GLFWWindow::update() {
  glfwPollEvents();
}

bool VulkanEngine::GLFWWindow::initialize() {

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

  glfwSetWindowUserPointer( glfw_window, this );
  glfwSetWindowSizeCallback( glfw_window, &windowResizeCallback );
  glfwSetFramebufferSizeCallback( glfw_window, &framebufferResizeCallback );

  glfwSetCursorPosCallback( glfw_window, &cursorPositionCallback );
  glfwSetMouseButtonCallback( glfw_window, &mouseButtonCallback );
  glfwSetKeyCallback( glfw_window, &keyCallback );

  mouse_input.reset( new MouseInput );

  return true;

}

vk::SurfaceKHR VulkanEngine::GLFWWindow::createVkSurface( const vk::Instance& instance ) {

  VkSurfaceKHR surface;
  VkResult err = glfwCreateWindowSurface( instance, glfw_window, nullptr, &surface );
  if( err != VK_SUCCESS ) {
    throw std::runtime_error( "Could not create Vulkan surface" );
  }

  return surface;

}

const std::vector< const char* > VulkanEngine::GLFWWindow::getRequiredVulkanInstanceExtensions() {

  uint32_t num_extensions;
  const char** extension_names;
  extension_names = glfwGetRequiredInstanceExtensions( &num_extensions );
  std::vector< const char* > extensions; 
  for( uint32_t i = 0; i < num_extensions; ++i ) {
    extensions.push_back( extension_names[i] );
  }
  return extensions;

}

bool VulkanEngine::GLFWWindow::shouldClose() {

  return glfwWindowShouldClose( glfw_window );

}

void VulkanEngine::GLFWWindow::setWidth( uint32_t _width ) {

  if( glfw_window ) {
    glfwSetWindowSize( glfw_window, static_cast< int >( _width ), NULL );
  } else {
    Window::setWidth( _width );
  }

}

void VulkanEngine::GLFWWindow::setHeight( uint32_t _height ) {

  if( glfw_window ) {
    glfwSetWindowSize( glfw_window, NULL, static_cast< int >( _height ) );
  } else {
    Window::setHeight( _height );
  }

}

void VulkanEngine::GLFWWindow::errorCallback( int error, const char* description ) {

  std::cerr << "GLFW error: " << description << " error code:" << error << std::endl;

}

void VulkanEngine::GLFWWindow::windowResizeCallback( GLFWwindow* _glfw_window, int _width, int _height ) {

  GLFWWindow* window = static_cast< GLFWWindow* >( glfwGetWindowUserPointer( _glfw_window ) );
  if( ( window->width != static_cast< uint32_t >( _width ) ) 
    || ( window->height != static_cast< uint32_t >( _height ) ) ) {
    window->size_changed = true;
  }
  window->width = static_cast< uint32_t >( _width );
  window->height = static_cast< uint32_t >( _height );

}

void VulkanEngine::GLFWWindow::framebufferResizeCallback( GLFWwindow* _glfw_window, int _width, int _height ) {

  GLFWWindow* window = static_cast< GLFWWindow* >( glfwGetWindowUserPointer( _glfw_window ) );
  if( ( window->framebuffer_width != static_cast< uint32_t >( _width ) ) 
    || ( window->framebuffer_height != static_cast< uint32_t >( _height ) ) ) {
    window->size_changed = true;
  }
  window->framebuffer_width = static_cast< uint32_t >( _width );
  window->framebuffer_height = static_cast< uint32_t >( _height );

}

void VulkanEngine::GLFWWindow::cursorPositionCallback(GLFWwindow* _glfw_window, double xpos, double ypos) {

  GLFWWindow* window = static_cast< GLFWWindow* >( glfwGetWindowUserPointer( _glfw_window ) );
  window->mousePositionCallback( xpos, ypos );

}

void VulkanEngine::GLFWWindow::mouseButtonCallback( GLFWwindow* _glfw_window, int button, int action, int mods ) {
  GLFWWindow* window = static_cast< GLFWWindow* >( glfwGetWindowUserPointer( _glfw_window ) );
  bool left = false;
  bool right = false;
  bool middle = false;

  if( button == GLFW_MOUSE_BUTTON_LEFT ) {
    left = action == GLFW_PRESS;
    right = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS;
    middle = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_MIDDLE ) == GLFW_PRESS;
  } else if( button == GLFW_MOUSE_BUTTON_RIGHT ) {
    left = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS;
    right = action == GLFW_PRESS;
    middle = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_MIDDLE ) == GLFW_PRESS;
  } else if( button == GLFW_MOUSE_BUTTON_MIDDLE ) {
    left = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS;
    right = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS;
    middle = action == GLFW_PRESS;
  } else {
    left = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS;
    right = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS;
    middle = glfwGetMouseButton( _glfw_window, GLFW_MOUSE_BUTTON_MIDDLE ) == GLFW_PRESS;
  }

  window->mouseButtonPressedCallback( left, middle, right );
}

void VulkanEngine::GLFWWindow::keyCallback( GLFWwindow* _glfw_window, int key, int scancode, int action, int mods ) {
}