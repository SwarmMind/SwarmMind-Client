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
#include <game/Camera.h>

using namespace std;

#pragma region initialization

void OpenGLRenderer::findUniformLocations()
{
	xLocation = glGetUniformLocation(program, "camX");
	yLocation = glGetUniformLocation(program, "camY");
	widthLocation = glGetUniformLocation(program, "camWidth");
	heightLocation = glGetUniformLocation(program, "camHeight");
}

OpenGLRenderer::OpenGLRenderer(GLFWwindow* _window, Camera& _camera)
	: window{_window}
	, particleRenderer{ _window, _camera }
	, camera {_camera}
	, commandRenderer {_camera}
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
	glUseProgram(program);
	glUniform1f(xLocation, camera.getX());
	glUniform1f(yLocation, camera.getY());

	glUniform1f(widthLocation, camera.getWidth());
	glUniform1f(heightLocation, camera.getHeight());
}

std::array<GLfloat, 6*5> spriteVertices(glm::vec3 pos, float width, float height, Sprite* sprite) {
	GLfloat u = sprite->u();
	GLfloat v = sprite->v();
	GLfloat u2 = sprite->u2();
	GLfloat v2 = sprite->v2();

	return std::array<GLfloat, 6*5> {
		pos.x,			pos.y,			pos.z,			u,	v,
		pos.x,			pos.y + height, pos.z,			u,	v2,
		pos.x + width,	pos.y,			pos.z,			u2,	v,
		pos.x,			pos.y + height, pos.z,			u,	v2,
		pos.x + width,	pos.y + height,	pos.z,			u2, v2,
		pos.x + width,	pos.y,			pos.z,			u2, v
    };
}

/**
*	\brief draws a sprite at the specified position
*	\param z has to be between 0 and 1, where 1 is the most "in front" and 0 is the most "in back"
*/
void OpenGLRenderer::drawSprite(glm::vec3 pos, float width, float height, Sprite* sprite)
{
	unsigned int vertsPerSprite = 6;
	unsigned int floatsPerVert = 5;
	unsigned int floatsPerSprite = floatsPerVert * vertsPerSprite;

	Texture* texture = sprite->texture();
	TextureRenderData& textureData = renderData[texture];
	
    const auto data = spriteVertices(pos, width, height, sprite);

	if (!textureData.addData(floatsPerSprite, data.data()))
	{
		drawTexture(texture);
		textureData.addData(floatsPerSprite, data.data());
	}
}

void OpenGLRenderer::drawCommandVisualizer(glm::vec3 pos, CommandVisualizer& visualizer)
{
	commandRenderer.drawCommandVisualizer(pos, visualizer);
}

void OpenGLRenderer::drawTexture(Texture* texture)
{
	TextureRenderData& textureData = renderData[texture];
	glBindTexture(GL_TEXTURE_2D, texture->ID());

	textureData.draw();
}

void OpenGLRenderer::preDraw()
{
	this->uploadCamera();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderData.clear();

	commandRenderer.preDraw();
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

	commandRenderer.draw();
	particleRenderer.draw(deltaTime);
}

void OpenGLRenderer::addParticles(ParticleSystem particles)
{
	particleRenderer.addParticles(particles);
}

#pragma endregion Drawing
