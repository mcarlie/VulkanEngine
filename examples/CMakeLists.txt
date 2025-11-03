# SimpleScene
file(GLOB EXAMPLES_SOURCES "SimpleScene/*.cpp" "SimpleScene/*.h")
add_executable(SimpleScene ${EXAMPLES_SOURCES})
target_include_directories(SimpleScene PRIVATE "${THIRDPARTY_DIR}/cxxopts-2.2.0")
target_link_libraries(SimpleScene VulkanEngine)
