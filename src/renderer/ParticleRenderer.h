#pragma once

#include <glbinding/gl41core/gl.h>
using namespace gl41core;
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>
#include <renderer/ParticleSystem.h>


class ParticleRenderer
{
public:
	ParticleRenderer(GLFWwindow* _window, class Camera& _camera);

	~ParticleRenderer();

	void draw(double deltaTime);

	void addParticles(ParticleSystem particles);

private:
	unsigned int particleCount;
	unsigned int textureSize = 256;
	GLfloat particleSize = 0.04;

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
	void addParticlesFromQueue();
	void updateParticles(double deltaTime);
	void drawParticles();
	void uploadCamera(int frameBufferWidth, int frameBufferHeight);

	GLuint cameraPositionUniform, cameraSizeUniform;

	class Camera& camera;
};
