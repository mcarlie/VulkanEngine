FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests IntegrationTests.cpp)

target_link_libraries(tests gtest_main VulkanEngine)

include(GoogleTest)
gtest_discover_tests(tests)

# Set the working directory for all tests to be able to access assets folder.
foreach(test ${tests})
  set_tests_properties(${test} PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
