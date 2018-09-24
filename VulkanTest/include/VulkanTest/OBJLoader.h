#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <VulkanTest/MeshBase.h>

#include <memory>

namespace VulkanTest {

  /// Fill a mesh with information from an obj file
  /// \tparam AdditionalAttributeType Any additional types of VertexAttribute types which the mesh will support
  /// \param obj_path The path to the file containing information about the mesh
  /// \param base_path The path to the folder containing associated mtl material files
  /// \param additional_attributes Additional attributes to give to the create mesh
  /// \return A vector of MeshBase instances where each MeshBase represents a shape retrieved from the obj file
  template< typename ... AdditionalAttributeTypes >
  std::vector< std::shared_ptr< MeshBase > > loadOBJ( 
    const char* obj_path,
    const char* base_path,
    AdditionalAttributeTypes ... additional_attributes );

}

#include <OBJLoader.cpp>

#endif /* OBJLOADER_H */