#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <glbinding/gl41core/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <lodepng/lodepng.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <game/Camera.h>
#include <renderer/Sprite.h>
#include <renderer/Texture.h>
#include <renderer/OpenGLHelpers.h>
#include <renderer/OpenGLRenderer.h>

using namespace std;

#pragma region initialization

void OpenGLRenderer::findUniformLocations()
{
	cameraPositionLocation = glGetUniformLocation(program, "camPosition");
	widthLocation = glGetUniformLocation(program, "camWidth");
	heightLocation = glGetUniformLocation(program, "camHeight");
	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
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
	glUniform2f(cameraPositionLocation, pos.x, pos.y);

	glUniform1f(widthLocation, m_camera.width());
	glUniform1f(heightLocation, m_camera.height());

	glm::mat3 view = glm::mat3(1.0f);
	//view = glm::translate(view, m_camera.position());
	view = glm::scale(view, glm::vec2{ 1/m_camera.width(), 1/m_camera.height() });
	glUniformMatrix3fv(viewLocation, 1, false, glm::value_ptr(view));
	glUniformMatrix3fv(modelLocation, 1, false, glm::value_ptr(view));
}

static std::array<GLfloat, 6*5> spriteVertices(glm::vec3 pos, float width, float height, std::shared_ptr<Sprite> sprite, glm::vec2 direction = glm::vec2(0.0, 1.0)) {
	const GLfloat u = sprite->u;
	const GLfloat v = sprite->v;
	const GLfloat u2 = sprite->u2;
	const GLfloat v2 = sprite->v2;

	const bool trivial = direction == glm::vec2{ 0.0,1.0 };
	const glm::vec2 center{ 0.5, 0.5 };
	const glm::vec2 center2{ width/2.0f, height/2.0f };
	const glm::vec2 extent{ width, height };
	const auto angle = glm::angle(glm::normalize(direction), glm::vec2(0.0, 1.0));
	//const glm::mat3 transform = glm::translate(glm::rotate(glm::translate(glm::mat3(1.0f), center), angle), -center);
	//const glm::mat3 transform = glm::translate(glm::scale(glm::rotate(glm::mat3(1.0f), angle), glm::vec2{ width, height }), glm::vec2{ pos });
	glm::mat3 transform(1.0f);
	transform = glm::scale(transform, extent);
	//transform = glm::translate(transform, center2);
	//transform = glm::rotate(transform, glm::pi<float>()); // angle);
	transform = glm::rotate(transform, angle);
	transform = glm::translate(transform, center2);
	//transform = glm::translate(transform, glm::vec2{ pos });
	std::array<GLfloat, 6*5> vertexData;

	struct Vertex {
		glm::vec2 p;
		glm::vec2 uv;
	};
	const float p = 0.5f, m = -0.5f;
	std::array<Vertex, 6> vertices{
		
		Vertex { glm::vec2{	m, m },	glm::vec2{ u,  v   } },
		Vertex { glm::vec2{ m, p },	glm::vec2{ u,  v2  } },
		Vertex { glm::vec2{	p, m },	glm::vec2{ u2, v   } },
		Vertex { glm::vec2{ m, p },	glm::vec2{ u,  v2  } },
		Vertex { glm::vec2{	p, p },	glm::vec2{ u2, v2  } },
		Vertex { glm::vec2{	p, m },	glm::vec2{ u2, v   } },
		
		/*
		Vertex { glm::vec2{	0.0f,	0.0f	},	glm::vec2{ u,  v   } },
		Vertex { glm::vec2{	0.0f,	height	},	glm::vec2{ u,  v2  } },
		Vertex { glm::vec2{	width,	0.0f	},	glm::vec2{ u2, v   } },
		Vertex { glm::vec2{	0.0f,	height	},	glm::vec2{ u,  v2  } },
		Vertex { glm::vec2{	width,	height	},	glm::vec2{ u2, v2  } },
		Vertex { glm::vec2{	width,	0.0f	},	glm::vec2{ u2, v   } },
		/*
		Vertex { glm::vec3{	pos.x,			pos.y,			pos.z}, glm::vec2{ u,  v   } },
		Vertex { glm::vec3{	pos.x,			pos.y + height, pos.z}, glm::vec2{ u,  v2  } },
		Vertex { glm::vec3{	pos.x + width,	pos.y,			pos.z}, glm::vec2{ u2, v   } },
		Vertex { glm::vec3{	pos.x,			pos.y + height, pos.z}, glm::vec2{ u,  v2  } },
		Vertex { glm::vec3{	pos.x + width,	pos.y + height,	pos.z}, glm::vec2{ u2, v2  } },
		Vertex { glm::vec3{	pos.x + width,	pos.y,			pos.z}, glm::vec2{ u2, v   } },
		*/
	};
	auto i = 0;
	for (auto& vertex: vertices) {
		vertex.p = pos + transform * glm::vec3{ vertex.p, 1.0f };
		vertexData[i++] = vertex.p.x;
		vertexData[i++] = vertex.p.y;
		vertexData[i++] = pos.z;
		vertexData[i++] = vertex.uv.x;
		vertexData[i++] = vertex.uv.y;
	}
	return vertexData;
}

/**
*	\brief draws a sprite at the specified position
*	\param z has to be between 0 and 1, where 1 is the most "in front" and 0 is the most "in back"
*/
void OpenGLRenderer::drawSprite(glm::vec3 pos, float width, float height, std::shared_ptr<Sprite> sprite, glm::vec2 direction)
{
	unsigned int vertsPerSprite = 6;
	unsigned int floatsPerVert = 5;
	unsigned int floatsPerSprite = floatsPerVert * vertsPerSprite;

	TextureRenderData& textureData = renderData[sprite->texture->ID()];
	
    const auto data = spriteVertices(pos, width, height, sprite, direction);

	if (!textureData.addData(floatsPerSprite, data.data()))
	{
		drawTexture(sprite->texture->ID());
		textureData.addData(floatsPerSprite, data.data());
	}
}

void OpenGLRenderer::drawSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite)
{
	drawSprite(pos, width, height, sprites.get(sprite), glm::vec2{ 0.0f, 1.0f });
}

void OpenGLRenderer::drawSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite, glm::vec2 direction)
{
	drawSprite(pos, width, height, sprites.get(sprite), direction);
}

Camera& OpenGLRenderer::camera()
{
    return m_camera;
}

void OpenGLRenderer::addStaticSprite(glm::vec3 pos, float width, float height, std::shared_ptr<Sprite> sprite)
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

void OpenGLRenderer::drawTexture(TextureID textureID)
{
	TextureRenderData& textureData = renderData[textureID];
	glBindTexture(GL_TEXTURE_2D, textureID);

	textureData.draw();
}

void OpenGLRenderer::preDraw()
{
	uploadCamera();
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
