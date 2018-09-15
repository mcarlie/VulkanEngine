#ifndef OBJLOADERLOADER_CPP
#define OBJLOADERLOADER_CPP

#include <VulkanTest/OBJLoader.h>

#include <tiny_obj_loader.h>

template< typename ... AdditionalAttributeTypes >
std::shared_ptr< VulkanTest::Renderable > VulkanTest::loadOBJ( const std::string& file_path, AdditionalAttributeTypes ... additional_types ) {

  std::shared_ptr< Mesh< float, uint16_t, additional_types ... > > mesh;

}

#endif /* OBJLOADER_CPP */