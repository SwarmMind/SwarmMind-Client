cmake_minimum_required(VERSION 3.8)
project(OpenGLEngine)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
add_subdirectory(src)

set_target_properties(OpenGLEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")