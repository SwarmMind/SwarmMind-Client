#include <renderer/ParticleRenderer.h>

#include <assert.h>
#include <renderer/OpenGLHelpers.h>
#include <renderer/Renderer.h>
#include <algorithm>
#include <vector>
#include <array>
#include <renderer/ParticleSystem.h>
#include <game/Camera.h>

#include <glbinding/Version.h>
#include <iostream>

ParticleRenderer::ParticleRenderer(GLFWwindow* window, Camera& camera)
	: m_window{window}
	, camera{camera}
{
	m_particleUpdateProgram = loadProgram("shaders/Passthrough.vert", "shaders/UpdateParticles.frag");
	m_particleDrawingProgram = loadProgram("shaders/ParticleRendering.vert", "shaders/ParticleRendering.frag");
	intializeUniforms();

	glGenTextures(2, m_dynamicParticleData);
	glGenFramebuffers(2, m_frameBufers);
	initializeDynamicData(m_front);
	initializeDynamicData(m_back);
	initializeStaticData();

	initializeUpdateVertexArray();
	initializeDrawVertexArray();
}

ParticleRenderer::~ParticleRenderer()
{
	glDeleteFramebuffers(2, m_frameBufers);
	glDeleteTextures(2, m_dynamicParticleData);
	glDeleteTextures(1, &m_particleColor);
	glDeleteTextures(1, &m_staticParticleData);

	glDeleteProgram(m_particleUpdateProgram);
	glDeleteProgram(m_particleDrawingProgram);

	glDeleteVertexArrays(1, &m_updateVao);
	glDeleteBuffers(1, &m_updateVbo);
	glDeleteVertexArrays(1, &m_drawVao);
	glDeleteBuffers(1, &m_drawVbo);
}

void ParticleRenderer::initializeStaticData()
{
	glGenTextures(1, &m_staticParticleData);
	glBindTexture(GL_TEXTURE_2D, m_staticParticleData);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)m_textureSize, (GLsizei)m_textureSize, 0, GL_RGBA, GL_FLOAT, nullptr);
	setTextureParameters();

	
	glGenTextures(1, &m_particleColor);
	glBindTexture(GL_TEXTURE_2D, m_particleColor);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)m_textureSize, (GLsizei)m_textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	setTextureParameters();
}

void ParticleRenderer::initializeDynamicData(unsigned int index)
{
	glBindTexture(GL_TEXTURE_2D, m_dynamicParticleData[index]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)m_textureSize, (GLsizei)m_textureSize, 0, GL_RGBA, GL_FLOAT, nullptr);
	setTextureParameters();

	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufers[index]);
	setFrameBufferTextures(index);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

	clearTexture();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ParticleRenderer::initializeUpdateVertexArray()
{
	glGenVertexArrays(1, &m_updateVao);
	glBindVertexArray(m_updateVao);

	glGenBuffers(1, &m_updateVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_updateVbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	std::vector<GLfloat> updateVertexData({
		-1, -1,
		1, -1,
		1, 1,
		-1, -1,
		1, 1,
		-1, 1
	});

	glBufferData(GL_ARRAY_BUFFER, 
		updateVertexData.size() * sizeof(GLfloat), 
		updateVertexData.data(), 
		GL_STATIC_DRAW);
}

void ParticleRenderer::initializeDrawVertexArray()
{
	glGenVertexArrays(1, &m_drawVao);
	glBindVertexArray(m_drawVao);

	glGenBuffers(1, &m_drawVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_drawVbo);
	
	std::vector<GLfloat> drawVertexData({
		-m_particleSize,	-m_particleSize,		-1, -1,
		m_particleSize,	-m_particleSize,		1, -1,
		m_particleSize,	m_particleSize,		1, 1,
		-m_particleSize,	-m_particleSize,		-1, -1,
		m_particleSize,	m_particleSize,		1, 1,
		-m_particleSize,	m_particleSize,		-1, 1
	});

	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBufferData(GL_ARRAY_BUFFER, 
		drawVertexData.size() * sizeof(GLfloat), 
		drawVertexData.data(), 
		GL_STATIC_DRAW);
}

void ParticleRenderer::intializeUniforms()
{
	m_deltaTimeUniform = glGetUniformLocation(m_particleUpdateProgram, "deltaTime");
	
	glUseProgram(m_particleUpdateProgram);
	GLuint staticDataSampler = glGetUniformLocation(m_particleUpdateProgram, "staticDataSampler");
	glUniform1i(staticDataSampler, 1);

	glUseProgram(m_particleDrawingProgram);
	GLuint textureSizeUniform = glGetUniformLocation(m_particleDrawingProgram, "particleTextureSize");
	glUniform1ui(textureSizeUniform, (GLuint)m_textureSize);
	GLuint particleColorSampler = glGetUniformLocation(m_particleDrawingProgram, "particleColorSampler");
	glUniform1i(particleColorSampler, 1);

	m_cameraPositionUniform = glGetUniformLocation(m_particleDrawingProgram, "cameraPosition");
	m_cameraSizeUniform = glGetUniformLocation(m_particleDrawingProgram, "cameraSize");
}

void ParticleRenderer::setTextureParameters()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //required for a framebuffer attachment!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //required for a framebuffer attachment!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}

void ParticleRenderer::clearTexture()
{
	GLfloat clearData[4] = { -2.f, -2.f, 0, 0 };
	glClearBufferfv(GL_COLOR, 0, clearData);
}

void ParticleRenderer::setFrameBufferTextures(int index)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_dynamicParticleData[index], 0);

	std::array<GLenum, 1> drawBuffers = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers((GLsizei)drawBuffers.size(), drawBuffers.data());
}

void ParticleRenderer::addParticlesFromQueue()
{
	std::queue<Particles>& particles = ParticleSystem::particlesToSpawn();
	while (!particles.empty())
	{
		addParticles(particles.front());
		particles.pop();
	}
}

void ParticleRenderer::updateParticles(double deltaTime)
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, (GLsizei)m_textureSize, (GLsizei)m_textureSize);
	glUseProgram(m_particleUpdateProgram);
	glBindVertexArray(m_updateVao);

	glUniform1f(m_deltaTimeUniform, static_cast<GLfloat>(deltaTime));

	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufers[m_back]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_staticParticleData);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_dynamicParticleData[m_front]); 

	glDrawArrays(GL_TRIANGLES, 0, 6);

	std::swap(m_front, m_back);
}

void ParticleRenderer::drawParticles()
{

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE); //only read from the depth buffer, do not write

	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);
	uploadCamera(width, height);

	glViewport(0, 0, width, height);
	glUseProgram(m_particleDrawingProgram);
	glBindVertexArray(m_drawVao);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_particleColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_dynamicParticleData[m_front]);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)(m_textureSize * m_textureSize));

	glDepthMask(GL_TRUE);
}

void ParticleRenderer::uploadCamera(int frameBufferWidth, int frameBufferHeight)
{
	glUseProgram(m_particleDrawingProgram);
	const auto pos = camera.position(), extent = camera.extent();
	glUniform2f(m_cameraPositionUniform, pos.x, pos.y);
	glUniform2f(m_cameraSizeUniform, extent.x, extent.y);
}

void ParticleRenderer::draw(double deltaTime)
{
	glDisable(GL_CULL_FACE);

	addParticlesFromQueue();
	updateParticles(deltaTime);
	drawParticles();
}

void ParticleRenderer::addParticles(Particles particles)
{
	assert(particles.m_dynamicData.size() % 4 == 0);
	assert(particles.m_dynamicData.size() == particles.m_staticData.size());
	assert(particles.m_dynamicData.size() == particles.m_color.size());

	size_t dataOffset = 0;
	size_t dataSize = particles.m_dynamicData.size() / 4;
	while (dataOffset < dataSize)
	{
		//write the particle data, one row at a time
		size_t xOffset = m_textureOffset % m_textureSize;
		size_t yOffset = m_textureOffset / m_textureSize	;
		size_t remainingWidth = std::min(m_textureSize - xOffset, dataSize - dataOffset);

		glBindTexture(GL_TEXTURE_2D, m_dynamicParticleData[m_front]);
		glTexSubImage2D(GL_TEXTURE_2D, 
			0, 
			(GLint)xOffset, 
			(GLint)yOffset,
			(GLsizei)remainingWidth, 
			1, 
			GL_RGBA, GL_FLOAT, 
			particles.m_dynamicData.data() + dataOffset * 4);

		glBindTexture(GL_TEXTURE_2D, m_staticParticleData);
		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			(GLint)xOffset,
			(GLint)yOffset,
			(GLsizei)remainingWidth,
			1,
			GL_RGBA,
			GL_FLOAT,
			particles.m_staticData.data() + dataOffset * 4);

		glBindTexture(GL_TEXTURE_2D, m_particleColor);
		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			(GLint)xOffset,
			(GLint)yOffset,
			(GLsizei)remainingWidth,
			1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			particles.m_color.data() + dataOffset * 4);
		
		m_textureOffset += remainingWidth;
		if (m_textureOffset >= m_textureSize * m_textureSize)
		{
			//Start writing to the texture again, if it is full.
			m_textureOffset = 0;
		}
		dataOffset += remainingWidth;
	}

}