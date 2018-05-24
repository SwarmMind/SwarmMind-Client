#pragma once

#include <glbinding/gl41core/gl.h>
using namespace gl41core;
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>

struct ParticleSystem {
	std::vector<GLfloat> dynamicData;
	std::vector<GLfloat> staticData;
	std::vector<GLubyte> color;
};

class ParticleRenderer
{
public:
	ParticleRenderer(GLFWwindow* _window, class Renderer& _renderer);

	~ParticleRenderer();

	void draw(double deltaTime);

	void addParticles(ParticleSystem particles);

private:
	unsigned int particleCount;
	unsigned int textureSize = 256;
	GLfloat particleSize = 0.05;

	unsigned int front = 0;
	unsigned int back = 1;
	GLuint dynamicParticleData[2];
	GLuint frameBuffers[2];
	GLuint staticParticleData;
	GLuint particleColor;

	GLuint particleUpdateProgram;
	GLuint particleDrawingProgram;

	GLuint updateVao;
	GLuint updateVbo;

	GLuint drawVao;
	GLuint drawVbo;

	GLFWwindow* window;

	GLuint deltaTimeUniform;

	unsigned int textureOffset = 0;

	//setup
	void initializeStaticData();
	void initializeDynamicData(unsigned int index);
	void clearTexture();
	void setTextureParameters();

	void initializeUpdateVertexArray();
	void initializeDrawVertexArray();
	
	void intializeUniforms();
	void setFrameBufferTextures(int index);

	//drawing
	void updateParticles(double deltaTime);
	void drawParticles();
	void uploadCamera(int frameBufferWidth, int frameBufferHeight);

	GLuint cameraPositionUniform, cameraSizeUniform;

	class Renderer& renderer;
};
