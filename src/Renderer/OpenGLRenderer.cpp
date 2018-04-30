#include <renderer/OpenGLRenderer.h>
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <lodepng/lodepng.h>
#include <renderer/Sprite.h>
#include <renderer/Texture.h>
#include <imgui/imgui.h>

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

void OpenGLRenderer::createVertexArray()
{
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);
}

void OpenGLRenderer::setVertexAttributes()
{
	floatsPerVert = 5;

	GLuint positionLocation = glGetAttribLocation(program, "position");
	glVertexArrayAttribBinding(vertexArrayObject, positionLocation, 0);
	glVertexArrayAttribFormat(vertexArrayObject, positionLocation, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vertexArrayObject, positionLocation);

	GLuint uvLocation = glGetAttribLocation(program, "uv");
	glVertexArrayAttribBinding(vertexArrayObject, uvLocation, 0);
	glVertexArrayAttribFormat(vertexArrayObject, uvLocation, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(vertexArrayObject, uvLocation);

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

	createVertexArray();
	setVertexAttributes();

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});
}

OpenGLRenderer::~OpenGLRenderer()
{
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vertexArrayObject);
}

#pragma endregion initialization

#pragma region Drawing
void OpenGLRenderer::drawSprite(float x, float y, float z, float width, float height, Sprite* sprite)
{
	unsigned int vertsPerSprite = 6;
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
	unsigned int vertexCount = textureData.bufferOffset / floatsPerVert;
	glBindTexture(GL_TEXTURE_2D, texture->ID());

	textureData.unmapBuffer();
	glBindBuffer(GL_ARRAY_BUFFER, textureData.vertexBuffer);
	glVertexArrayVertexBuffer(vertexArrayObject, 0, textureData.vertexBuffer, 0, floatsPerVert * sizeof(GLfloat));
	
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void OpenGLRenderer::setCamera(float x, float y, float height)
{
	glUniform1f(xLocation, x);
	glUniform1f(yLocation, y);

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	float width = (static_cast<float>(bufferWidth) / static_cast<float>(bufferHeight)) * height;
	
	glUniform1f(widthLocation, width);
	glUniform1f(heightLocation, height);
}

void OpenGLRenderer::preDraw()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::draw(bool remapBuffer /*= false*/)
{	
	for (auto iterator = renderData.begin(); iterator != renderData.end(); iterator++)
	{
		drawTexture(iterator->first);
	}
}

#pragma endregion Drawing
