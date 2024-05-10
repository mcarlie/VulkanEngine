#include <VulkanEngine/ShaderModule.h>
#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Device.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <fstream>
#include <iostream>

bool VulkanEngine::ShaderModule::glslang_initialized = false;

VulkanEngine::ShaderModule::ShaderModule(
    const std::string &shader_string, bool is_filepath,
    vk::ShaderStageFlagBits shader_stage_flag)
    : vk_shader_stage_flag(shader_stage_flag) {
  std::vector<uint32_t> bytecode;

  if (!is_filepath) {
    glslToSPIRV("unknown", shader_string, bytecode);
  } else {
    // Determine if this is a file path and attempt to open it if it is.
    std::filesystem::path shader_path(shader_string);
    if (std::filesystem::exists(shader_path)) {
      readSource(shader_path, bytecode);
    }
  }

  if (bytecode.empty()) {
    throw std::runtime_error("Unable to read shader source.");
  }

  auto shader_module_info = vk::ShaderModuleCreateInfo()
                                .setPCode(bytecode.data())
                                .setCodeSize(bytecode.size());

  vk_shader_module =
      VulkanManager::getInstance().getDevice()->getVkDevice().createShaderModule(
          shader_module_info);
  if (!vk_shader_module) {
    throw std::runtime_error("Could not create ShaderModule!");
  }
}

VulkanEngine::ShaderModule::~ShaderModule() {
  VulkanManager::getInstance().getDevice()->getVkDevice().destroyShaderModule(
      vk_shader_module);
}

const vk::ShaderStageFlagBits
VulkanEngine::ShaderModule::getVkShaderStageFlag() const {
  return vk_shader_stage_flag;
}

const vk::ShaderModule &VulkanEngine::ShaderModule::getVkShaderModule() const {
  return vk_shader_module;
}

void VulkanEngine::ShaderModule::readSource(std::filesystem::path file_path,
                                            std::vector<uint32_t> &bytecode) {
  // Already compiled just read data
  if (file_path.extension() == ".spv") {
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open shader file " +
                               file_path.string());
    }

    const size_t file_size = static_cast<size_t>(file.tellg());
    bytecode.resize(file_size);
    file.seekg(0);
    file.read(reinterpret_cast<char *>(bytecode.data()), file_size);
    file.close();

    return;
  }

  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open shader file " +
                             file_path.string());
  }

  std::string glsl_string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());

  glslToSPIRV(file_path.string(), glsl_string, bytecode);
}

void VulkanEngine::ShaderModule::glslToSPIRV(const std::string &name,
                                             const std::string &shader_string,
                                             std::vector<uint32_t> &bytecode) {
  if (!glslang_initialized) {
    glslang::InitializeProcess();
    glslang_initialized = true;
  }

  EShLanguage shader_type;
  switch (vk_shader_stage_flag) {
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
    throw std::runtime_error("Invalid shader stage flag!");
  }

  glslang::TShader tshader(shader_type);
  const char *glsl_c_str = shader_string.c_str();
  tshader.setStrings(&glsl_c_str, 1);
  glslang::EshTargetClientVersion client_version =
      glslang::EShTargetVulkan_1_0; // TODO
  glslang::EShTargetLanguageVersion language_version =
      glslang::EShTargetSpv_1_0;

  tshader.setEnvInput(glslang::EShSourceGlsl, shader_type,
                      glslang::EShClientVulkan,
                      450); // TODO 100 means vulkan 1.0
  tshader.setEnvClient(glslang::EShClientVulkan, client_version);
  tshader.setEnvTarget(glslang::EShTargetSpv, language_version);

  const TBuiltInResource *resources = GetDefaultResources();
  EShMessages messages =
      static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
  glslang::TShader::ForbidIncluder
      includer; /// TODO google has an includer in the source for glslang. Use
                /// that? Write own?

  std::string preprocessed_glsl;
  /// TODO 100 is default glsl version
  if (!tshader.preprocess(resources, 450, ENoProfile, false, false, messages,
                          &preprocessed_glsl, includer)) {
    std::cout << "Preprocessing failed for shader " + name << std::endl;
    std::cout << tshader.getInfoLog() << std::endl;
    std::cout << tshader.getInfoDebugLog() << std::endl;
  }

  const char *preprocessed_glsl_c_str = preprocessed_glsl.c_str();
  tshader.setStrings(&preprocessed_glsl_c_str, 1);

  /// TODO 100 is default glsl version
  if (!tshader.parse(resources, 450, false, messages)) {
    std::cout << "Parsing failed for shader " + name << std::endl;
    std::cout << tshader.getInfoLog() << std::endl;
    std::cout << tshader.getInfoDebugLog() << std::endl;
  }

  glslang::TProgram tprogram;
  tprogram.addShader(&tshader);

  if (!tprogram.link(messages)) {
    std::cout << "Linking failed for shader " + name << std::endl;
    std::cout << tprogram.getInfoLog() << std::endl;
    std::cout << tprogram.getInfoDebugLog() << std::endl;
  }

  std::vector<uint32_t> spirv_data;
  spv::SpvBuildLogger spv_logger;
  glslang::SpvOptions spv_options;
  glslang::GlslangToSpv(*tprogram.getIntermediate(shader_type), spirv_data,
                        &spv_logger, &spv_options);

  bytecode.resize(spirv_data.size() * (sizeof(unsigned int) / sizeof(char)));
  std::memcpy(bytecode.data(), spirv_data.data(), bytecode.size());
}
