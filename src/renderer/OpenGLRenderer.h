#pragma once
#include <renderer/Renderer.h>
#include <string>
#include <glbinding/gl41core/gl.h>
#include <glm/vec3.hpp>
#include <map>
#include <unordered_map>
#include <vector>
#include <renderer/TextureRenderData.h>
#include <renderer/ParticleRenderer.h>
#include <renderer/CommandVisualizationRenderer.h>
#include <renderer/StaticRenderData.h>

using namespace gl41core;
using namespace std;

struct GLFWwindow;

class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer(GLFWwindow* _window, Camera& _camera);
	~OpenGLRenderer();
	
	void preDraw();

	virtual void drawSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite, float rotation = 0) override;
	virtual void drawSprite(glm::vec3 pos, float width, float height, std::shared_ptr<Sprite> sprite, float rotation = 0) override;
	virtual void drawCommandVisualizer(glm::vec3 pos, CommandVisualizer& visualizer) override;
	void draw(double deltaTime);

    virtual Camera& camera() override;

    virtual void addStaticSprite(glm::vec3 pos, float width, float height, std::shared_ptr<Sprite> sprite, float rotation = 0) override;
    virtual void addStaticSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite, float rotation = 0) override;
    virtual void clearStaticData() override;

private:
	virtual void uploadCamera();

	void findUniformLocations();

	GLint program;

    //static renderData
    std::vector<StaticRenderData> m_staticRenderData;

	//Texture render data
	std::unordered_map<TextureID, TextureRenderData> renderData;
	void drawTexture(TextureID texture);

	//Camera
	GLFWwindow* window = nullptr;
	GLint xLocation;
	GLint yLocation;
	GLint widthLocation;
	GLint heightLocation;
	class Camera& m_camera;
    

	ParticleRenderer particleRenderer;
	CommandVisualizationRenderer commandRenderer;

	//////////////////////////////////////////////////////////////////////////
	//					command visualization
	//////////////////////////////////////////////////////////////////////////
	GLuint commandVisualizationVertexArray;
	GLuint positionVertexBuffer, colorVertexBuffer;
	GLfloat* mappedPositionBuffer = nullptr;
	GLubyte* mappedColorBuffer = nullptr;

	Textures textures;
	Sprites sprites;

    std::array<GLfloat, 6 * 5> spriteVertices(glm::vec3 pos, float rotation, float width, float height, std::shared_ptr<Sprite> sprite);
    glm::vec2 rotatePoint(glm::vec2 point, glm::vec2 around, float angle);
};