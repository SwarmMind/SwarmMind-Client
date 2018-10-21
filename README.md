# SwarmMind-Client

[![Build status](https://ci.appveyor.com/api/projects/status/1bdud8dmgn9i03l6?svg=true)](https://ci.appveyor.com/project/hesiod/swarmmind-client)

The client for the SwarmMind game, written in OpenGL and C++, connects to the server via Socket.IO

## Download prebuilt windows executable
You can also download a prebuilt Windows x64 executable on [appveyor](https://ci.appveyor.com/project/hesiod/swarmmind-client).
Just select the latest builds "Release" job, select "artifacts" and download the SwarmMind-Client.zip.
Lastly, simply extract the zip-file and run SwarmMind-Client.exe

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
   You might also need to install the boost-asio and openssl packages via vcpkg, if you encounter any issues try installing those.
   
3. Specify the toolchain file location on the CMake command line: `cmake -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake path/to/source/tree`\
If the boost version that was installed by vcpkg is >1.67, then you need to update cmake to version 3.13 or above.(Only a release candidate is available at the time of writing)\
If cmake cannot find your installed packages, even if you provide the correct CMAKE_TOOLCHAIN_FILE, make sure that cmake is using the correct generator (x64 or x32). The generator must match the bit-count of your installed packages. For example if you installed the x64 packages on windows you would use the `-G "Visual Studio 15 2017 Win64"` cmake option. Run `cmake -G` to get more information on the available generators.
