#pragma once

#include <vector>
#include <queue>
#include <glbinding/gl41core/gl.h>
#include <glm/fwd.hpp>
using namespace gl41core;

class ParticleSystem
{
public:
	ParticleSystem();

	~ParticleSystem();

	std::vector<GLfloat> dynamicData;
	std::vector<GLfloat> staticData;
	std::vector<GLubyte> color;

private:
	void addParticle(
		GLfloat x, GLfloat y, 
		GLfloat xVelocity, GLfloat yVelocity, 
		GLfloat lifetime, 
		GLubyte r, GLubyte g, GLubyte b, GLubyte alpha);

	void addParticle(glm::vec2 position, glm::vec2 velocity, float lifeTime, glm::vec4 color);

	//////////////////////////////////////////////////////////////////////////
	//						static
	//////////////////////////////////////////////////////////////////////////
public:

	static void addParticles(ParticleSystem particles);
	static void spawnTestParticles(float x, float y, float width, float height);
	static void spawnBloodParticles(glm::vec2 position, glm::vec2 direction);
	static void mouseDragParticles(glm::vec2 mousePosition, glm::vec2 target, glm::vec4 color, double deltaTime);
	static void spawnAcknowledgeParticles(glm::vec2 position);

	static std::queue<ParticleSystem>& particlesToSpawn();

private:
	static std::queue<ParticleSystem> particleQueue;
	
	//helper functions for random
	static float randomFloat();
	static void randomizeColor(GLubyte& r, GLubyte& g, GLubyte b, GLubyte maximumDeviation);
	template<class T>
	static T clamp(T value, T minimum, T maximum);

	static float randomFloatBetween(float minimum, float maximum);

};
