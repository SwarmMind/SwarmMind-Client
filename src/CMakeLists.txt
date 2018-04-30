set(sources "main.cpp")

add_subdirectory(renderer)
add_subdirectory(imgui)
add_subdirectory(lodepng)

add_executable(OpenGLEngine ${sources})

#Replicate the directory structure in Visual Studio filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
target_link_libraries(OpenGLEngine PUBLIC glbinding::glbinding glfw)
