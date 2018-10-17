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

	void addParticles(Particles particles);

private:
	size_t m_particleCount;
	size_t m_textureSize = 256;
	GLfloat m_particleSize = 0.04f;

	unsigned int m_front = 0;
	unsigned int m_back = 1;
	GLuint m_dynamicParticleData[2];
	GLuint m_frameBufers[2];
	GLuint m_staticParticleData;
	GLuint m_particleColor;

	GLuint m_particleUpdateProgram;
	GLuint m_particleDrawingProgram;

	GLuint m_updateVao;
	GLuint m_updateVbo;

	GLuint m_drawVao;
	GLuint m_drawVbo;

	GLFWwindow* m_window;

	GLuint m_deltaTimeUniform;

	size_t m_textureOffset = 0;

	GLuint m_cameraPositionUniform, m_cameraSizeUniform;

	class Camera& camera;
	
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
};
