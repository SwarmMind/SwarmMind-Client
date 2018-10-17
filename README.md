# SwarmMind-Client

[![Build status](https://ci.appveyor.com/api/projects/status/1bdud8dmgn9i03l6?svg=true)](https://ci.appveyor.com/project/hesiod/swarmmind-client)

The client for the SwarmMind game, written in OpenGL and C++, connects to the server via Socket.IO

## Build using vcpkg
Using vcpkg is recommended since it makes building all the dependencies a breeze.
1. Follow the [vcpkg installation instructions](https://github.com/Microsoft/vcpkg), including user-wide integration
2. Install the required dependencies for your platform:
```
# For x64
vcpkg --triplet x64-windows install glm glfw3 sfml glbinding boost-random boost-system boost-date-time boost-exception
# For x86
vcpkg --triplet x86-windows install glm glfw3 sfml glbinding boost-random boost-system boost-date-time boost-exception
```
3. Specify the toolchain file location on the CMake command line: `cmake -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake path/to/source/tree`

## Build instructions for manual build
The client uses CMake (v3.8 required) as its build system and has the following dependencies:
1. [GLFW 3.2.1](http://www.glfw.org/)
2. [glbinding](https://github.com/cginternals/glbinding)
3. A [fork](https://github.com/SwarmMind/socket.io-client-cpp) (with some fixes and build systems improvements) of the [socket.io C++ client](https://github.com/socketio/socket.io-client-cpp)
4. [glm](https://glm.g-truc.net/0.9.9/index.html)

### Building GLFW
Follow the [build instructions](http://www.glfw.org/docs/latest/compile_guide.html#compile_generate) on the GLFW site to build the library using Cmake.

On Windows:
* Make sure to set the `CMAKE_INSTALL_DIR` to the directory, you want your generated Cmake files to be copied to.
* Build the INSTALL project in Visual Studio on Release (x64/x32 depending on your operating system)

### Building glbinding
Download [glbinding 3.0.2](https://github.com/cginternals/glbinding/releases/tag/v3.0.2) and extract the files into any directory.
In this directory create a /build folder and build glbinding into it.
Compile glbinding using your favorite compiler.

### Building the socket.io C++ client
## Instructions for Windows
1. Please visit our Slack and download and unzip sioclient.zip

## Instructions for Linux/macOS
1. Install [boost](https://boost.org) (using your favorite package manager or by building it from scratch)
2. Clone the [socket.io C++ client fork](https://github.com/SwarmMind/socket.io-client-cpp)
3. build the library using cmake
4. compile the library
5. Ensure you have the library (`sioclient.a`) and the include directory
6. Create a new folder and copy the files so they form the following folder structure:
* include
  * sio_client.h
  * sio_message.h
  * sio_socket.h
* lib
  * Debug
    * sioclient.a
    * libboost_date_time_ ... .a
    * libboost_random_ ... .a
    * libboost_system_ ... .a
  * Release
    * sioclient.a
    * libboost_date_time_ ... .a
    * libboost_random_ ... .a
    * libboost_system_ ... .a

Note: on Windows, the folder structure should look the same but all files ending with .a should end with .lib

### Building glm
Just download and unzip Version 0.9.9.0 of [glm](https://glm.g-truc.net/0.9.9/index.html).
Run Cmake in the folder you unzipped glm to, make sure to set GLM_INSTALL_ENABLE to true.

### Adding the libraries and building the client
1. Download the repository (either by pulling from the repository or downloading the zip file).
2. Go to the repositories directory and run cmake.
3. Set the glfw3_DIR variable to "your-glfw3-install-dir"/lib/cmake/glfw3
4. Set the glbinding_DIR to "your-glbinding-dir"
5. Set the SIOCLIENT_DIR to "your-sioclient-dir" (this directory must have a folder structure as described in the building socket.IO C++ client section)
6. Generate the cmake files and compile the project
7. Copy the compiled OpenGLEngine.exe to your SwarmMind-Client directory
8. Copy the compiled glbdinding.dll/glbindingd.dll (use the d Variant if your building on Debug) next to the OpenGLEngine.exe
8. Run the OpenGLEngine.exe
