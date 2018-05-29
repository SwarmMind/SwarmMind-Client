# SwarmMind-Client
The Client for the SwarmMind game, written in OpenGL and C++, connects to the server via Socket.IO

## Build Instructions
The SwarmMind-Client uses CMake (v.3.8 required) as its build system and has the following dependencies:
1. [GLFW 3.2.1](http://www.glfw.org/)
2. [glbinding](https://github.com/cginternals/glbinding)
3. [socket.IO c++ client](https://github.com/socketio/socket.io-client-cpp)
4. [glm](https://glm.g-truc.net/0.9.9/index.html)

### Building GLFW
Follow the [build instructions](http://www.glfw.org/docs/latest/compile_guide.html#compile_generate) on the GLFW site to build the library using Cmake.

On Windows:
* Make sure to set the CMAKE_INSTALL_DIR to the directory, you want your generated Cmake files to be copied to.
* Build the INSTALL project in Visual Studio on Release (x64/x32 depending on your operating system)

### Building glbinding
Download [glbinding 2.1.4](https://github.com/cginternals/glbinding/releases/tag/v2.1.4) and extract the files into any directory.
In this directory create a /build folder and build glbinding into it.
Compile glbinding using your favorite compiler.

### Building the socket.IO C++ client
For SwarmMind-developers on Windows:
1. Please visit our slack and download and unzip sioclient.zip

For SwarmMind-developers on Unix:
1. Install [boost](boost.org) (using your favorite package manager or by building it from scratch)
2. clone the [socket.IO c++ client](https://github.com/socketio/socket.io-client-cpp)
3. build the library using cmake
4. compile the library
5. you should now have a sioclient.a file and the include directory for the socket.IO client
6. create a folder and copy the files so they form the following folder structure:
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

### Adding the libraries and building the SwarmMind-Client
1. Download the repository (either by pulling from the repository or downloading the zip file).
2. Go to the repositories directory and run cmake.
3. Set the glfw3_DIR variable to "your-glfw3-install-dir"/lib/cmake/glfw3
4. Set the glbinding_DIR to "your-glbinding-dir"
5. Set the SIOCLIENT_DIR to "your-sioclient-dir" (this directory must have a folder structure as described in the building socket.IO C++ client section)
6. Generate the cmake files and compile the project
7. Copy the compiled OpenGLEngine.exe to your SwarmMind-Client directory
8. Copy the compiled glbdinding.dll/glbindingd.dll (use the d Variant if your building on Debug) next to the OpenGLEngine.exe
8. Run the OpenGLEngine.exe
