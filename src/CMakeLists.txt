set(sources "main.cpp")

add_subdirectory(renderer)
add_subdirectory(IMGUI)
add_subdirectory(lodepng)

add_executable(OpenGLEngine ${sources})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})

set(GLFW_DIRECTORY "" CACHE PATH "Path to GLFW")
add_subdirectory(${GLFW_DIRECTORY})
include_directories(${GLFW_DIRECTORY}/include)

target_link_libraries(OpenGLEngine glfw)

find_package(glbinding REQUIRED)
target_link_libraries(OpenGLEngine PUBLIC glbinding::glbinding)
