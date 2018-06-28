find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(glm REQUIRED)

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

add_executable(SwarmMind-Client ${sources})

target_link_libraries(SwarmMind-Client PRIVATE imgui)

#Replicate the directory structure in Visual Studio filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})

#Find and include the socketio cpp bindings
set(SIOCLIENT_DIR "" CACHE PATH "The sioclient library directory")
set(SIOCLIENT_USE_FIND_PACKAGE OFF CACHE BOOL "Use find_package for sioclient")

if(SIOCLIENT_USE_FIND_PACKAGE)
    find_package(sioclient 1.6.0 REQUIRED)
    target_include_directories(SwarmMind-Client PRIVATE sioclient)
    target_link_libraries(SwarmMind-Client PRIVATE sioclient)
else()
if(EXISTS ${SIOCLIENT_DIR})
	target_include_directories(SwarmMind-Client PUBLIC ${SIOCLIENT_DIR}/include)
	
	#find all libraries in the SIOCLIENT lib file
	file(GLOB SIOCLIENT_LIBRARY_DIR_DEBUG
		${SIOCLIENT_DIR}/lib/Debug/*.lib
		${SIOCLIENT_DIR}/lib/Debug/*.a
	)
	file(GLOB SIOCLIENT_LIBRARY_DIR_RELEASE
		${SIOCLIENT_DIR}/lib/Release/*.lib
		${SIOCLIENT_DIR}/lib/Release/*.a
	)
	#Add every library in a loop, because the debug and optimized keywords only work for one lib at a time
	foreach(SIOCLIENT_LIB_DEBUG ${SIOCLIENT_LIBRARY_DIR_DEBUG})
		target_link_libraries(SwarmMind-Client PUBLIC
								debug ${SIOCLIENT_LIB_DEBUG})
	endforeach(SIOCLIENT_LIB_DEBUG)
	foreach(SIOCLIENT_LIB_RELEASE ${SIOCLIENT_LIBRARY_DIR_RELEASE})
		target_link_libraries(SwarmMind-Client PUBLIC
								optimized ${SIOCLIENT_LIB_RELEASE})
	endforeach(SIOCLIENT_LIB_RELEASE)
else()
	message(FATAL_ERROR "sioclient directory not set!")
endif()
endif()

target_link_libraries(SwarmMind-Client PUBLIC glbinding::glbinding glfw glm)

if(MSVC)
target_compile_options(SwarmMind-Client PRIVATE /wd4251)
endif()

