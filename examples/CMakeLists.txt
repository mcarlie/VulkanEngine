# SimpleScene
file(GLOB EXAMPLES_SOURCES "SimpleScene/*.cpp" "SimpleScene/*.h")
add_executable(SimpleScene ${EXAMPLES_SOURCES})
target_include_directories(SimpleScene PRIVATE "${THIRDPARTY_DIR}/cxxopts-2.2.0")
target_link_libraries(SimpleScene VulkanEngine)

# Triangle
file(GLOB TRIANGLE_SOURCES "Triangle/*.cpp" "Triangle/*.h")
add_executable(Triangle ${TRIANGLE_SOURCES})
target_link_libraries(Triangle VulkanEngine)
