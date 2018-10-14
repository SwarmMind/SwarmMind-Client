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
#include <cmath>

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


std::array<GLfloat, 6*5> OpenGLRenderer::spriteVertices(glm::vec3 pos, float rotation, float width, float height, std::shared_ptr<Sprite> sprite) {
	GLfloat u = sprite->u;
	GLfloat v = sprite->v;
	GLfloat u2 = sprite->u2;
	GLfloat v2 = sprite->v2;

    glm::vec2 pivot = glm::vec2(pos.x, pos.y) + glm::vec2(width / 2.f, height / 2.f);

    glm::vec2 bottomLeft = rotatePoint(pos, pivot, rotation);
    glm::vec2 topLeft = rotatePoint(pos + glm::vec3(0.f, height, 0.f), pivot, rotation);
    glm::vec2 bottomRight = rotatePoint(pos + glm::vec3(width, 0.f, 0.f), pivot, rotation);
    glm::vec2 topRight = rotatePoint(pos + glm::vec3(width, height, 0.f), pivot, rotation);

	return std::array<GLfloat, 6*5> {
		bottomLeft.x,	bottomLeft.y,   pos.z,			u,	v,
		topLeft.x,		topLeft.y,      pos.z,			u,	v2,
		bottomRight.x,	bottomRight.y,  pos.z,			u2,	v,
		topLeft.x,		topLeft.y,      pos.z,			u,	v2,
		topRight.x, 	topRight.y, 	pos.z,			u2, v2,
		bottomRight.x,	bottomRight.y,	pos.z,			u2, v
    };
}

glm::vec2 OpenGLRenderer::rotatePoint(glm::vec2 point, glm::vec2 around, float angle)
{
    angle = 0 - angle;

    float s = sin(angle);
    float c = cos(angle);

    // translate point back to origin:
    point -= around;

    // rotate point
    float xnew = point.x * c - point.y * s;
    float ynew = point.x * s + point.y * c;

    // translate point back:
    return glm::vec2(xnew, ynew) + around;
}

/**
*	\brief draws a sprite at the specified position
*	\param z has to be between 0 and 1, where 1 is the most "in front" and 0 is the most "in back"
*/
void OpenGLRenderer::drawSprite(glm::vec3 pos, float width, float height, std::shared_ptr<Sprite> sprite, float rotation)
{
	unsigned int vertsPerSprite = 6;
	unsigned int floatsPerVert = 5;
	unsigned int floatsPerSprite = floatsPerVert * vertsPerSprite;

	TextureRenderData& textureData = renderData[sprite->texture->ID()];
	
    const auto data = spriteVertices(pos, rotation, width, height, sprite);

	if (!textureData.addData(floatsPerSprite, data.data()))
	{
		drawTexture(sprite->texture->ID());
		textureData.addData(floatsPerSprite, data.data());
	}
}

void OpenGLRenderer::drawSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite, float rotation)
{
	drawSprite(pos, width, height, sprites.get(sprite), rotation);
}

Camera& OpenGLRenderer::camera()
{
    return m_camera;
}

void OpenGLRenderer::addStaticSprite(glm::vec3 pos, float width, float height, std::shared_ptr<Sprite> sprite, float rotation)
{
    unsigned int vertsPerSprite = 6;
    unsigned int floatsPerVert = 5;
    unsigned int floatsPerSprite = floatsPerVert * vertsPerSprite;

    auto iterator = std::find_if(m_staticRenderData.begin(), m_staticRenderData.end(), [=](StaticRenderData& data) {
        return sprite->texture->ID() == data.texture()->ID();
    });
    bool found = iterator != m_staticRenderData.end();
    if (!found)
    {
        m_staticRenderData.emplace_back(StaticRenderData(sprite->texture));
    }
    StaticRenderData& renderData = found ? *iterator : m_staticRenderData.back();

    auto data = spriteVertices(pos, rotation, width, height, sprite);
    renderData.addData(vertsPerSprite, data.data());
}

void OpenGLRenderer::addStaticSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite, float rotation)
{
    addStaticSprite(pos, width, height, sprites.get(sprite), rotation);
}

void OpenGLRenderer::clearStaticData()
{
    m_staticRenderData.clear();
}

void OpenGLRenderer::drawCommandVisualizer(glm::vec3 pos, CommandVisualizer& visualizer)
{
	commandRenderer.drawCommandVisualizer(pos, visualizer);
}

void OpenGLRenderer::drawTexture(TextureID textureID)
{
	TextureRenderData& textureData = renderData[textureID];
	glBindTexture(GL_TEXTURE_2D, textureID);

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
