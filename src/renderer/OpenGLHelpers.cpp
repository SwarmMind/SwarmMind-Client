#include <renderer/OpenGLHelpers.h>
#include <glbinding/gl41core/gl.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
using namespace gl41core;
using namespace std;

#pragma region Shader loading

std::string loadShaderFile(string path)
{
	string shader;
	ifstream file(path);
	if (!file.is_open())
	{
		cout << "Could not open file: " << path << endl;
		exit(-1);
	}

	while (!file.eof())
	{
		char character = file.get();
		if (character > 0)
			shader += character;
	}
	return shader;
}

GLint loadShader(string path, GLenum shaderType)
{
	string code = loadShaderFile(path);
	const GLchar* source = code.data();
	const GLint length = code.length();
	GLint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, &length);
	glCompileShader(shader);

	GLboolean compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		GLint errorLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);

		vector<GLchar> infoLog(errorLength);
		glGetShaderInfoLog(shader, errorLength, &errorLength, &infoLog[0]);

		cout << "Shader compilation: " << path << " failed:\n" << infoLog.data();

		glDeleteShader(shader);
		exit(-1);
	}

	return shader;
}

GLint loadProgram(string vertexShaderPath, string fragmentShaderPath)
{
	GLint vertexShader = loadShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLint fragmentShader = loadShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	GLint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLboolean isLinked;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);

		cout << "OpenGL Linker Error:" << std::endl;
		cout << errorLog.data();
		glDeleteProgram(program);
		exit(-1);
	}

	return program;
}

#pragma  endregion