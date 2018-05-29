#pragma once
#include <renderer/Renderer.h>
#include <string>
#include <glbinding/gl41core/gl.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <renderer/TextureRenderData.h>
#include <renderer/ParticleRenderer.h>

using namespace gl41core;
using namespace std;

struct GLFWwindow;

class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer(GLFWwindow* _window, Camera& _camera);
	~OpenGLRenderer();
	
	void preDraw();

	virtual void drawSprite(float x, float y, float z, float width, float height, Sprite* sprite) override;
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
};