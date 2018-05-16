#include <renderer/OpenGLRenderer.h>
#include <glbinding/gl41core/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <lodepng/lodepng.h>
#include <renderer/Sprite.h>
#include <renderer/Texture.h>
#include <imgui/imgui.h>

using namespace std;

#pragma region Shader loading

GLint OpenGLRenderer::loadProgram(string vertexShaderPath, string fragmentShaderPath)
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

GLint OpenGLRenderer::loadShader(string path, GLenum shaderType)
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

std::string OpenGLRenderer::loadShaderFile(string path)
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

#pragma  endregion

#pragma region initialization

void OpenGLRenderer::findUniformLocations()
{
	xLocation = glGetUniformLocation(program, "camX");
	yLocation = glGetUniformLocation(program, "camY");
	widthLocation = glGetUniformLocation(program, "camWidth");
	heightLocation = glGetUniformLocation(program, "camHeight");
}

OpenGLRenderer::OpenGLRenderer(GLFWwindow* _window)
	: window{_window}
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glDepthRange(0.0, 1.0);

	program = loadProgram("shaders/vert.glsl", "shaders/frag.glsl");
	glUseProgram(program);

	findUniformLocations();
	
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});
}

OpenGLRenderer::~OpenGLRenderer()
{
	glDeleteProgram(program);
}

#pragma endregion initialization

#pragma region Drawing
void OpenGLRenderer::uploadCamera()
{
	//Update the size every frame
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	float width = (static_cast<float>(bufferWidth) / static_cast<float>(bufferHeight)) * camera.height;
	camera.width = width;

	glUniform1f(xLocation, camera.x);
	glUniform1f(yLocation, camera.y);

	glUniform1f(widthLocation, camera.width);
	glUniform1f(heightLocation, camera.height);

}

/**
*	\brief draws a sprite at the specified position
*	\param z has to be between 0 and 1, where 1 is the most "in front" and 0 is the most "in back"
*/
void OpenGLRenderer::drawSprite(float x, float y, float z, float width, float height, Sprite* sprite)
{
	unsigned int vertsPerSprite = 6;
	unsigned int floatsPerVert = 5;
	unsigned int floatsPerSprite = floatsPerVert * vertsPerSprite;

	Texture* texture = sprite->texture();
	TextureRenderData& textureData = renderData[texture];
	
	GLfloat u = sprite->u();
	GLfloat v = sprite->v();
	GLfloat u2 = sprite->u2();
	GLfloat v2 = sprite->v2();

	GLfloat data[] = { 
		x,			y,			z,			u,	v,
		x,			y + height, z,			u,	v2,
		x + width,	y,			z,			u2,	v,
		x,			y + height, z,			u,	v2,
		x + width,	y + height,	z,			u2, v2,
		x + width,	y,			z,			u2, v };

	if (!textureData.addData(floatsPerSprite, data))
	{
		drawTexture(texture);
		textureData.addData(floatsPerSprite, data);
	}
}

void OpenGLRenderer::drawTexture(Texture* texture)
{
	TextureRenderData& textureData = renderData[texture];
	glBindTexture(GL_TEXTURE_2D, texture->ID());

	textureData.draw();
}

/**
	\brief set the view, the renderer should display
	The width is calculated automatically according to the current aspect ratio
	This command is best called before calling preDraw in the current frame, as otherwise the camera will not be updated for the current frame
*/
void OpenGLRenderer::setCamera(float x, float y, float height)
{
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	float width = (static_cast<float>(bufferWidth) / static_cast<float>(bufferHeight)) * height;

	camera.x = x;
	camera.y = y;
	camera.height = height;
	camera.width = width;
}

Camera OpenGLRenderer::getCamera()
{
	return camera;
}

void OpenGLRenderer::preDraw()
{
	this->uploadCamera();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::draw()
{	
	for (auto iterator = renderData.begin(); iterator != renderData.end(); iterator++)
	{
		drawTexture(iterator->first);
	}
}

#pragma endregion Drawing
