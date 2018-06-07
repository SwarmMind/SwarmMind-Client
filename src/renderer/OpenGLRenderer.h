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

using namespace gl41core;
using namespace std;

struct GLFWwindow;

class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer(GLFWwindow* _window, Camera& _camera);
	~OpenGLRenderer();
	
	void preDraw();

	virtual void drawSprite(glm::vec3 pos, float width, float height, Sprite* sprite) override;
	virtual void drawCommandVisualizer(glm::vec3 pos, CommandVisualizer& visualizer) override;
	void draw(double deltaTime);

	void addParticles(ParticleSystem particles);

private:
	virtual void uploadCamera();

	void findUniformLocations();

	GLint program;

	//Texture render data
	std::unordered_map<class Texture*, TextureRenderData> renderData;
	void drawTexture(class Texture* texture);

	//Camera
	GLFWwindow* window = nullptr;
	GLint xLocation;
	GLint yLocation;
	GLint widthLocation;
	GLint heightLocation;
	class Camera& camera;

	ParticleRenderer particleRenderer;
	CommandVisualizationRenderer commandRenderer;

	//////////////////////////////////////////////////////////////////////////
	//					command visualization
	//////////////////////////////////////////////////////////////////////////
	GLuint commandVisualizationVertexArray;
	GLuint positionVertexBuffer, colorVertexBuffer;
	GLfloat* mappedPositionBuffer = nullptr;
	GLubyte* mappedColorBuffer = nullptr;

	void mapBuffer();
};