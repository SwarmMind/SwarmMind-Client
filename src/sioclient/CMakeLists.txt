cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
PROJECT(sioclient
        VERSION 1.6.0
        LANGUAGES CXX)

include(CMakePackageConfigHelpers)

set(CMAKE_DEBUG_POSTFIX d)

option(BUILD_SHARED_LIBS "Build the shared library" OFF)
option(Boost_USE_STATIC_LIBS "Use Boost static version" ON)

set(BOOST_VER "1.55.0" CACHE STRING "boost version" )

set(Boost_USE_MULTITHREADED ON) 
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost ${BOOST_VER} REQUIRED COMPONENTS system date_time random exception)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src ALL_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/internal ALL_SRC)
file(GLOB ALL_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/*.h )
set(SIO_INCLUDEDIR ${CMAKE_CURRENT_LIST_DIR})

# Fix MSVC build (https://github.com/Nelson-numerical-software/socket.io-client-cpp)
if(MSVC_VERSION EQUAL 1900 OR MSVC_VERSION GREATER 1900)
add_definitions(-D__STDC_LIMIT_MACROS -D_WEBSOCKETPP_NOEXCEPT_ -D_WEBSOCKETPP_CPP11_CHRONO_)
endif()

add_library(sioclient ${ALL_SRC})
set_property(TARGET sioclient PROPERTY CXX_STANDARD 11)
set_property(TARGET sioclient PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(sioclient
	PROPERTIES
		SOVERSION ${PROJECT_VERSION_MAJOR}
		VERSION ${PROJECT_VERSION}
	)

target_include_directories(sioclient PRIVATE ${Boost_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_LIST_DIR}/src 
    ${CMAKE_CURRENT_LIST_DIR}/lib/websocketpp 
    ${CMAKE_CURRENT_LIST_DIR}/lib/rapidjson/include
)
target_link_libraries(sioclient PUBLIC ${Boost_LIBRARIES})
target_include_directories(sioclient PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

find_package(OpenSSL)
if(OPENSSL_FOUND)
add_library(sioclient_tls ${ALL_SRC})
target_include_directories(sioclient_tls PRIVATE ${Boost_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_LIST_DIR}/src 
    ${CMAKE_CURRENT_LIST_DIR}/lib/websocketpp 
    ${CMAKE_CURRENT_LIST_DIR}/lib/rapidjson/include
    ${OPENSSL_INCLUDE_DIR}
)

set_property(TARGET sioclient_tls PROPERTY CXX_STANDARD 11)
set_property(TARGET sioclient_tls PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(sioclient_tls PUBLIC ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} )
target_include_directories(sioclient_tls PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_definitions(sioclient_tls PRIVATE -DSIO_TLS)
set_target_properties(sioclient_tls
	PROPERTIES
		SOVERSION ${PROJECT_VERSION_MAJOR}
		VERSION ${PROJECT_VERSION}
	)
endif()

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/sioclient-config-version.cmake
                                 COMPATIBILITY AnyNewerVersion)

install(FILES ${ALL_HEADERS} DESTINATION include)
install(TARGETS sioclient
        EXPORT sioclient
        ARCHIVE
          DESTINATION lib
        LIBRARY
          DESTINATION lib)
install(TARGETS sioclient_tls
        EXPORT sioclient
        ARCHIVE
          DESTINATION lib
        LIBRARY
          DESTINATION lib)
install(EXPORT sioclient
        DESTINATION lib/cmake/sioclient
        FILE sioclient-config.cmake)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sioclient-config-version.cmake
        DESTINATION lib/cmake/sioclient)

