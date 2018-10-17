#pragma once

#include <vector>
#include <queue>
#include <random>

#include <glbinding/gl41core/gl.h>
#include <glm/fwd.hpp>
using namespace gl41core;

struct Particles
{
	std::vector<GLfloat> m_dynamicData;
	std::vector<GLfloat> m_staticData;
	std::vector<GLubyte> m_color;

	void addParticle(
		GLfloat x, GLfloat y,
		GLfloat xVelocity, GLfloat yVelocity,
		GLfloat lifetime,
		GLubyte r, GLubyte g, GLubyte b, GLubyte alpha);

	void addParticle(glm::vec2 position, glm::vec2 velocity, float lifeTime, glm::vec4 color);
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	//////////////////////////////////////////////////////////////////////////
	//						static
	//////////////////////////////////////////////////////////////////////////
	static void addParticles(Particles particles);
	static void spawnTestParticles(float x, float y, float width, float height);
	static void spawnBloodParticles(glm::vec2 position, glm::vec2 direction);
	static void spawnShootParticles(glm::vec2 position, glm::vec2 direction);
	static void mouseDragParticles(glm::vec2 mousePosition, glm::vec2 target, glm::vec4 color, double deltaTime);
	static void spawnAcknowledgeParticles(glm::vec2 position);

	static std::queue<Particles>& particlesToSpawn();

private:
	static std::queue<Particles> particleQueue;
	
	//helper functions for random
	static glm::vec4 randomizeColor(glm::vec4 color, float maximumDeviation, bool randomizeAlpha = false);
	
	static float randomFloatBetween(float minimum, float maximum);

	static glm::vec2 randomOffset(float maximum);
};
