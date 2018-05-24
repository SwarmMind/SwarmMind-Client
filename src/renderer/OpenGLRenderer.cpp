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
#include <renderer/OpenGLHelpers.h>

using namespace std;

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
	, particleRenderer{ _window, *this }
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glDepthRange(0.0, 1.0);

	program = loadProgram("shaders/Sprite.vert", "shaders/Sprite.frag");
	glUseProgram(program);

	findUniformLocations();
	
	/*glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});*/
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

	glUseProgram(program);
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
	renderData.clear();
}

void OpenGLRenderer::draw(double deltaTime)
{
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(program);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	for (auto iterator = renderData.begin(); iterator != renderData.end(); iterator++)
	{
		drawTexture(iterator->first);
	}

	particleRenderer.draw(deltaTime);
}

void OpenGLRenderer::addParticles(ParticleSystem particles)
{
	particleRenderer.addParticles(particles);
}

#pragma endregion Drawing
