#pragma once
#include <renderer/Renderer.h>
#include <string>
#include <glbinding/gl/gl.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <renderer/TextureRenderData.h>

using namespace std;
using namespace gl;

struct GLFWwindow;

class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer(GLFWwindow* window);
	~OpenGLRenderer();

	virtual void drawSprite(float x, float y, float z, float width, float height, Sprite* sprite) override;
	virtual void setCamera(float x, float y, float height) override;

	void preDraw();
	void draw(bool remapBuffer = false);

private:
	GLint loadProgram(string vertexShader, string fragmentShader);
	GLint loadShader(string path, GLenum shaderType);
	string loadShaderFile(string path);

	void createVertexArray();
	void setVertexAttributes();

	unsigned int floatsPerVert;
	GLuint vertexArrayObject;
	GLint program;

	//Texture render data
	unordered_map<class Texture*, TextureRenderData> renderData;
	void drawTexture(class Texture* texture);

	//Camera
	GLFWwindow* window = nullptr;
	GLint xLocation;
	GLint yLocation;
	GLint widthLocation;
	GLint heightLocation;
};