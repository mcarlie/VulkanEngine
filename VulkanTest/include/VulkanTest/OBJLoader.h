#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <VulkanTest/Renderable.h>

#include <memory>

namespace VulkanTest {

  /// Fill a mesh with information from an obj file
  /// \param file_path The path to the file containing information about the mesh
  /// \return The loaded mesh
  template< typename ... AdditionalAttributeTypes >
  std::shared_ptr< Renderable > loadOBJ( const std::string& file_path, AdditionalAttributeTypes ... additional_types );

}

#include <OBJLoader.cpp>

#endif /* MESHLOADER_H */