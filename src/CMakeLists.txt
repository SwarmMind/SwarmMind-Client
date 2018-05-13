set(sources "main.cpp")

add_subdirectory(renderer)
add_subdirectory(imgui)
add_subdirectory(lodepng)
add_subdirectory(nlohmann)
add_subdirectory(gamestate)

add_executable(OpenGLEngine ${sources})

#Replicate the directory structure in Visual Studio filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})

#Find and include the socketio cpp bindings
set(SIOCLIENT_DIR "" CACHE PATH "The sioclient library directory")
if(EXISTS ${SIOCLIENT_DIR})
	target_include_directories(OpenGLEngine PUBLIC ${SIOCLIENT_DIR}/include)
	
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
		target_link_libraries(OpenGLEngine PUBLIC
								debug ${SIOCLIENT_LIB_DEBUG})
	endforeach(SIOCLIENT_LIB_DEBUG)
	foreach(SIOCLIENT_LIB_RELEASE ${SIOCLIENT_LIBRARY_DIR_RELEASE})
		target_link_libraries(OpenGLEngine PUBLIC
								optimized ${SIOCLIENT_LIB_RELEASE})
	endforeach(SIOCLIENT_LIB_RELEASE)
else()
	message(FATAL_ERROR "sioclient directory not set!")
endif()


find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)

target_link_libraries(OpenGLEngine PUBLIC glbinding::glbinding glfw)
