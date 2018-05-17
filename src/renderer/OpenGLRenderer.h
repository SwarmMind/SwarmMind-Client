#pragma once
#include <renderer/Renderer.h>
#include <string>
#include <glbinding/gl41core/gl.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <renderer/TextureRenderData.h>

using namespace gl41core;
using namespace std;

struct GLFWwindow;

class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer(GLFWwindow* window);
	~OpenGLRenderer();
	
	void preDraw();

	virtual void drawSprite(float x, float y, float z, float width, float height, Sprite* sprite) override;
	virtual void setCamera(float x, float y, float height) override;
	virtual Camera getCamera() override;
	void draw();

private:
	Camera camera;
	virtual void uploadCamera();

	GLint loadProgram(string vertexShader, string fragmentShader);
	GLint loadShader(string path, GLenum shaderType);
	std::string loadShaderFile(string path);

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
};