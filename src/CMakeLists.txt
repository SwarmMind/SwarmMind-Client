find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(glm REQUIRED)
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS audio REQUIRED)

set(sources "main.cpp")

add_subdirectory(renderer)
add_subdirectory(imgui)
add_subdirectory(lodepng)
add_subdirectory(nlohmann)
add_subdirectory(gamestate)
add_subdirectory(input)
add_subdirectory(game)
add_subdirectory(menu)
add_subdirectory(events)
add_subdirectory(sound)

add_executable(SwarmMind-Client ${sources})

target_link_libraries(SwarmMind-Client PRIVATE imgui sfml-audio)

#Replicate the directory structure in Visual Studio filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})

#Find and include the socketio cpp bindings
find_package(sioclient 1.6.0 REQUIRED)
target_include_directories(SwarmMind-Client PRIVATE sioclient)
target_link_libraries(SwarmMind-Client PRIVATE sioclient)

target_link_libraries(SwarmMind-Client PUBLIC glbinding::glbinding glbinding::glbinding-aux glfw glm)

if(MSVC)
target_compile_options(SwarmMind-Client PRIVATE /wd4251)
endif()

