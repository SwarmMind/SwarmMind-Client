#pragma once
#include <glbinding/gl41core/gl.h>
#include <string>
using namespace gl41core;


std::string loadShaderFile(std::string path);
GLint loadShader(std::string path, GLenum shaderType);
GLint loadProgram(std::string vertexShaderPath, std::string fragmentShaderPath);
