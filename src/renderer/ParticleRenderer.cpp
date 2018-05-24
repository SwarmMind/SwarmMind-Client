#include <renderer/ParticleRenderer.h>

#include <assert.h>
#include <renderer/OpenGLHelpers.h>
#include <algorithm>
#include <vector>
#include <array>

ParticleRenderer::ParticleRenderer(GLFWwindow* _window)
	: window{_window}
{
	front = 0;
	back = 1;
	
	glGenTextures(2, dynamicParticleData);
	glGenFramebuffers(2, frameBuffers);

	particleUpdateProgram = loadProgram("shaders/Passthrough.vert", "shaders/UpdateParticles.frag");
	particleDrawingProgram = loadProgram("shaders/ParticleRendering.vert", "shaders/ParticleRendering.frag");
	intializeUniforms();

	initializeDynamicData(front);
	initializeDynamicData(back);
	initializeStaticData();

	initializeUpdateVertexArray();
	initializeDrawVertexArray();
}

ParticleRenderer::~ParticleRenderer()
{
	glDeleteTextures(2, dynamicParticleData);
	glDeleteFramebuffers(2, frameBuffers);
	glDeleteProgram(particleUpdateProgram);
	glDeleteProgram(particleDrawingProgram);
	glDeleteVertexArrays(1, &updateVao);
	glDeleteBuffers(1, &updateVbo);
}

void ParticleRenderer::initializeStaticData()
{
	glGenTextures(1, &staticParticleData);
	glBindTexture(GL_TEXTURE_2D, staticParticleData);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureSize, textureSize, 0, GL_RGBA, GL_FLOAT, nullptr);
	setTextureParameters();

	
	glGenTextures(1, &particleColor);
	glBindTexture(GL_TEXTURE_2D, particleColor);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureSize, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	setTextureParameters();
}

void ParticleRenderer::initializeDynamicData(unsigned int index)
{
	glBindTexture(GL_TEXTURE_2D, dynamicParticleData[index]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureSize, textureSize, 0, GL_RGBA, GL_FLOAT, nullptr);
	setTextureParameters();

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[index]);
	setFrameBufferTextures(index);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);

	clearTexture();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ParticleRenderer::initializeUpdateVertexArray()
{
	glGenVertexArrays(1, &updateVao);
	glBindVertexArray(updateVao);

	glGenBuffers(1, &updateVbo);
	glBindBuffer(GL_ARRAY_BUFFER, updateVbo);

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
	glGenVertexArrays(1, &drawVao);
	glBindVertexArray(drawVao);

	glGenBuffers(1, &drawVbo);
	glBindBuffer(GL_ARRAY_BUFFER, drawVbo);
	
	std::vector<GLfloat> drawVertexData({
		-particleSize,	-particleSize,		-1, -1,
		particleSize,	-particleSize,		1, -1,
		particleSize,	particleSize,		1, 1,
		-particleSize,	-particleSize,		-1, -1,
		particleSize,	particleSize,		1, 1,
		-particleSize,	particleSize,		-1, 1
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
	deltaTimeUniform = glGetUniformLocation(particleUpdateProgram, "deltaTime");
	
	glUseProgram(particleUpdateProgram);
	GLuint staticDataSampler = glGetUniformLocation(particleUpdateProgram, "staticDataSampler");
	glUniform1i(staticDataSampler, 1);

	glUseProgram(particleDrawingProgram);
	GLuint textureSizeUniform = glGetUniformLocation(particleDrawingProgram, "particleTextureSize");
	glUniform1ui(textureSizeUniform, textureSize);
	GLuint particleColorSampler = glGetUniformLocation(particleDrawingProgram, "particleColorSampler");
	glUniform1i(particleColorSampler, 1);
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
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dynamicParticleData[index], 0);

	std::array<GLenum, 1> drawBuffers = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(drawBuffers.size(), drawBuffers.data());
}

void ParticleRenderer::updateParticles(double deltaTime)
{
	glDisable(GL_BLEND);
	glViewport(0, 0, textureSize, textureSize);
	glUseProgram(particleUpdateProgram);
	glBindVertexArray(updateVao);

	glUniform1f(deltaTimeUniform, deltaTime);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[back]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, staticParticleData);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dynamicParticleData[front]); 

	glDrawArrays(GL_TRIANGLES, 0, 6);

	std::swap(front, back);
}

void ParticleRenderer::drawParticles()
{
	glEnable(GL_BLEND);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glUseProgram(particleDrawingProgram);
	glBindVertexArray(drawVao);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, particleColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dynamicParticleData[front]);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, textureSize * textureSize);
}

void ParticleRenderer::draw(double deltaTime)
{
	//Important for updating the particles
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	updateParticles(deltaTime);
	drawParticles();
}

void ParticleRenderer::addParticles(ParticleSystem particles)
{
	assert(particles.dynamicData.size() % 4 == 0);
	assert(particles.dynamicData.size() == particles.staticData.size());
	assert(particles.dynamicData.size() == particles.color.size());

	unsigned int dataOffset = 0;
	unsigned int dataSize = particles.dynamicData.size() / 4;
	while (dataOffset < dataSize)
	{
		//write the particle data, one row at a time
		unsigned int xOffset = textureOffset % textureSize;
		unsigned int yOffset = textureOffset / textureSize	;
		unsigned int remainingWidth = std::min(textureSize - xOffset, dataSize - dataOffset);

		glBindTexture(GL_TEXTURE_2D, dynamicParticleData[front]);
		glTexSubImage2D(GL_TEXTURE_2D, 
			0, 
			xOffset, 
			yOffset, 
			remainingWidth, 
			1, 
			GL_RGBA, GL_FLOAT, 
			particles.dynamicData.data() + dataOffset * 4);

		glBindTexture(GL_TEXTURE_2D, staticParticleData);
		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			xOffset,
			yOffset,
			remainingWidth,
			1,
			GL_RGBA,
			GL_FLOAT,
			particles.staticData.data() + dataOffset * 4);

		glBindTexture(GL_TEXTURE_2D, particleColor);
		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			xOffset,
			yOffset,
			remainingWidth,
			1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			particles.color.data() + dataOffset * 4);
		
		textureOffset += remainingWidth;
		if (textureOffset >= textureSize * textureSize)
		{
			//Start writing to the texture again, if it is full.
			textureOffset = 0;
		}
		dataOffset += remainingWidth;
	}

}