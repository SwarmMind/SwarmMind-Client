# SwarmMind-Client
The Client for the SwarmMind game, written in OpenGL and C++, connects to the server via Socket.IO

## Build Instructions
The SwarmMind-Client uses CMake (v.3.8 required) as its build system and has the following dependencies:
1. [GLFW 3.2.1](http://www.glfw.org/)
2. [glbinding](https://github.com/cginternals/glbinding)

### Building GLFW
Follow the [build instructions](http://www.glfw.org/docs/latest/compile_guide.html#compile_generate) on the GLFW site to build the library using Cmake.

On Windows:
* Make sure to set the CMAKE_INSTALL_DIR to the directory, you want your generated Cmake files to be copied to.
* Build the INSTALL project in Visual Studio on Release (x64/x32 depending on your operating system)

### Building glbinding
Download [glbinding 2.1.4](https://github.com/cginternals/glbinding/releases/tag/v2.1.4) and extract the files into any directory.
In this directory create a /build folder and build glbinding into it.
Compile glbinding using your favorite build tool.

### Adding the libraries and building the SwarmMind-Client
1. Download the repository (either by pulling from the repository or downloading the zip file).
2. Go to the repositories directory and run cmake.
3. Set the glfw3_DIR variable to <your-glfw3-install-dir>/lib/cmake/glfw3
4. Set the glbinding_DIR to <your-glbinding-dir>
5. Generate the cmake files and compile the project
6. Copy the compiled OpenGLEngine.exe to your SwarmMind-Client directory
7. Run the OpenGLEngine.exe
