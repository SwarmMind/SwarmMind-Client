#include <renderer/OpenGLRenderer.h>
#include <glbinding/gl41core/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
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
	, m_camera {_camera}
	, particleRenderer{ window, m_camera }
	, commandRenderer { m_camera}
	, textures{}
	, sprites{textures}
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glDepthRange(0.0, 1.0);

	program = loadProgram("shaders/Sprite.vert", "shaders/Sprite.frag");
	glUseProgram(program);

	findUniformLocations();
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
	const auto pos = m_camera.position();
	glUniform1f(xLocation, pos.x);
	glUniform1f(yLocation, pos.y);

	glUniform1f(widthLocation, m_camera.width());
	glUniform1f(heightLocation, m_camera.height());
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

void OpenGLRenderer::drawSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite)
{
	drawSprite(pos, width, height, sprites.get(sprite));
}

Camera& OpenGLRenderer::camera()
{
    return m_camera;
}

void OpenGLRenderer::addStaticSprite(glm::vec3 pos, float width, float height, Sprite* sprite)
{
    unsigned int vertsPerSprite = 6;
    unsigned int floatsPerVert = 5;
    unsigned int floatsPerSprite = floatsPerVert * vertsPerSprite;

    Texture* texture = sprite->texture();
   
    auto iterator = std::find_if(m_staticRenderData.begin(), m_staticRenderData.end(), [=](StaticRenderData& data) {
        return texture->ID() == data.texture()->ID();
    });
    bool found = iterator != m_staticRenderData.end();
    if (!found)
    {
        m_staticRenderData.emplace_back(StaticRenderData(texture));
    }
    StaticRenderData& renderData = found ? *iterator : m_staticRenderData.back();

    auto data = spriteVertices(pos, width, height, sprite);
    renderData.addData(vertsPerSprite, data.data());
}

void OpenGLRenderer::addStaticSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite)
{
    addStaticSprite(pos, width, height, sprites.get(sprite));
}

void OpenGLRenderer::clearStaticData()
{
    m_staticRenderData.clear();
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

    for (auto& staticRenderData : m_staticRenderData)
    {
        staticRenderData.draw();
    }

	for (auto& TextureDataPair : renderData)
	{
		drawTexture(TextureDataPair.first);
	}


	commandRenderer.draw();
	particleRenderer.draw(deltaTime);
}

#pragma endregion Drawing
