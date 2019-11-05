#include <VulkanEngine/ShaderModule.h>
#include <VulkanEngine/VulkanManager.h>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.cpp>

#include <fstream>
#include <iostream>

bool VulkanEngine::ShaderModule::glslang_initialized = false;

VulkanEngine::ShaderModule::ShaderModule( const std::string& file_path, vk::ShaderStageFlagBits shader_stage_flag ) 
  : vk_shader_stage_flag( shader_stage_flag ) {

  std::vector< char > bytecode;
  readSource( file_path, bytecode );

  auto shader_module_info = vk::ShaderModuleCreateInfo()
    .setPCode( reinterpret_cast< const uint32_t* >( bytecode.data() ) )
    .setCodeSize( bytecode.size() );

  vk_shader_module = VulkanManager::getInstance()->getVkDevice().createShaderModule( shader_module_info );
  if( !vk_shader_module ) {
    throw std::runtime_error( "Could not create ShaderModule!" );
  }

}

VulkanEngine::ShaderModule::~ShaderModule() {
  VulkanManager::getInstance()->getVkDevice().destroyShaderModule( vk_shader_module );
}

const vk::ShaderStageFlagBits VulkanEngine::ShaderModule::getVkShaderStageFlag() {
  return vk_shader_stage_flag;
}

const vk::ShaderModule& VulkanEngine::ShaderModule::getVkShaderModule() {
  return vk_shader_module;
}

void VulkanEngine::ShaderModule::readSource( const std::string& file_path, std::vector< char >& bytecode ) {
  
  std::string segment;
  std::stringstream path_string_stream( file_path );
  std::vector< std::string > split_string;
  while( std::getline( path_string_stream, segment, '.' ) ) {
    split_string.push_back( segment );
  }
  
  // Already compiled just read data
  if( split_string.back() == "spv" ) {
    
    std::ifstream file( file_path, std::ios::ate | std::ios::binary );
    if( !file.is_open() ) {
      std::cerr << "Could not open shader file " << file_path << std::endl;
      return;
    }
  
    const size_t file_size = static_cast< size_t >( file.tellg() );
    bytecode.resize( file_size );
    file.seekg( 0 );
    file.read( bytecode.data(), file_size );
    file.close();
    
    return;
    
  }
  
  std::ifstream file( file_path );
  if( !file.is_open() ) {
    throw std::runtime_error( "Could not open shader file " + file_path );
  }
  
  std::string glsl_string( ( std::istreambuf_iterator< char >( file ) ),
                          std::istreambuf_iterator< char >( ) );
  
  std::vector< unsigned int > spirv_data;
  glslToSPIRV( file_path, glsl_string, spirv_data );
  
  bytecode.resize( spirv_data.size() * ( sizeof( unsigned int ) / sizeof( char ) ) );
  std::memcpy( bytecode.data(), spirv_data.data(), bytecode.size() );

}

void VulkanEngine::ShaderModule::glslToSPIRV(
  const std::string& file_path,
  const std::string& shader_string,
  std::vector< unsigned int >& spirv ) {
 
  if( !glslang_initialized ) {
    glslang::InitializeProcess();
    glslang_initialized = true;
  }
  
  EShLanguage shader_type;
  switch( vk_shader_stage_flag ) {
    case vk::ShaderStageFlagBits::eVertex:
      shader_type = EShLangVertex;
      break;
    case vk::ShaderStageFlagBits::eTessellationControl:
      shader_type = EShLangTessControl;
      break;
    case vk::ShaderStageFlagBits::eTessellationEvaluation:
      shader_type = EShLangTessEvaluation;
      break;
    case vk::ShaderStageFlagBits::eGeometry:
      shader_type = EShLangGeometry;
      break;
    case vk::ShaderStageFlagBits::eFragment:
      shader_type = EShLangFragment;
      break;
    case vk::ShaderStageFlagBits::eCompute:
      shader_type = EShLangCompute;
      break;
    case vk::ShaderStageFlagBits::eRaygenNV:
      shader_type = EShLangRayGenNV;
      break;
    case vk::ShaderStageFlagBits::eAnyHitNV:
      shader_type = EShLangAnyHitNV;
      break;
    case vk::ShaderStageFlagBits::eClosestHitNV:
      shader_type = EShLangClosestHitNV;
      break;
    case vk::ShaderStageFlagBits::eMissNV:
      shader_type = EShLangMissNV;
      break;
    case vk::ShaderStageFlagBits::eIntersectionNV:
      shader_type = EShLangIntersectNV;
      break;
    case vk::ShaderStageFlagBits::eCallableNV:
      shader_type = EShLangCallableNV;
      break;
    case vk::ShaderStageFlagBits::eTaskNV:
      shader_type = EShLangTaskNV;
      break;
    case vk::ShaderStageFlagBits::eMeshNV:
      shader_type = EShLangMeshNV;
      break;
    default:
      throw std::runtime_error( "Invalid shader stage flag" );
  }
  
  glslang::TShader tshader( shader_type );
  const char* glsl_c_str = shader_string.c_str();
  tshader.setStrings( &glsl_c_str, 1 );
  glslang::EshTargetClientVersion client_version = glslang::EShTargetVulkan_1_0; // TODO
  glslang::EShTargetLanguageVersion language_version = glslang::EShTargetSpv_1_0;
  
  tshader.setEnvInput( glslang::EShSourceGlsl, shader_type, glslang::EShClientVulkan, 100 ); // TODO 100 means vulkan 1.0
  tshader.setEnvClient(glslang::EShClientVulkan, client_version);
  tshader.setEnvTarget(glslang::EShTargetSpv, language_version);
  
  TBuiltInResource resources = glslang::DefaultTBuiltInResource;
  EShMessages messages = static_cast< EShMessages >( EShMsgSpvRules | EShMsgVulkanRules );
  glslang::TShader::ForbidIncluder includer; /// TODO google has an includer in the source for glslang. Use that? Write own?
  
  std::string preprocessed_glsl;
  /// TODO 100 is default glsl version
  if( !tshader.preprocess( &resources, 100, ENoProfile, false, false, messages, &preprocessed_glsl, includer ) ) {
    std::cout << "Preprocessing failed for shader " + file_path << std::endl;
    std::cout << tshader.getInfoLog() << std::endl;
    std::cout << tshader.getInfoDebugLog() << std::endl;
  }
  
  const char* preprocessed_glsl_c_str = preprocessed_glsl.c_str();
  tshader.setStrings( &preprocessed_glsl_c_str, 1 );
  
  /// TODO 100 is default glsl version
  if( !tshader.parse( &resources, 100, false, messages ) ) {
    std::cout << "Parsing failed for shader " + file_path << std::endl;
    std::cout << tshader.getInfoLog() << std::endl;
    std::cout << tshader.getInfoDebugLog() << std::endl;
  }
  
  glslang::TProgram tprogram;
  tprogram.addShader( &tshader );
  
  if( !tprogram.link( messages ) ) {
    std::cout << "Linking failed for shader " + file_path << std::endl;
    std::cout << tprogram.getInfoLog() << std::endl;
    std::cout << tprogram.getInfoDebugLog() << std::endl;
  }
  
  spv::SpvBuildLogger spv_logger;
  glslang::SpvOptions spv_options;
  glslang::GlslangToSpv( *tprogram.getIntermediate( shader_type ), spirv, &spv_logger, &spv_options );
  
}
