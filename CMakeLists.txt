cmake_minimum_required(VERSION 3.8)
project(SwarmMind-Client)

set(CMAKE_CXX_STANDARD 14)

foreach(path ${CMAKE_PREFIX_PATH})
  message("Using Prefix Path: " ${path})
endforeach(path)

include_directories(src)
add_subdirectory(src)

set_target_properties(SwarmMind-Client PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")