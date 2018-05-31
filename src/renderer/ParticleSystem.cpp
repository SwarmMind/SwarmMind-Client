#include <renderer/ParticleSystem.h>
#include <random>
#include <glm/glm.hpp>
#include <algorithm>

ParticleSystem::ParticleSystem()
{

}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::addParticles(ParticleSystem particles)
{
	particleQueue.push(particles);
}

void ParticleSystem::spawnTestParticles(float x, float y, float width, float height)
{
	ParticleSystem particles;
	for (size_t i = 0; i < 10; i++)
	{
		GLfloat directionX = randomFloatBetween(-width, width), directionY = randomFloatBetween(-height, height);
		GLfloat lifeTime = randomFloatBetween(0.5, 1.5);
		GLubyte r = randomFloat() * 255, g = randomFloat() * 255, b = randomFloat() * 255, alpha = 255;

		particles.addParticle(x, y, directionX, directionY, lifeTime, r, g, b, alpha);
	}
	addParticles(particles);
}

void ParticleSystem::spawnBloodParticles(float x, float y)
{
	ParticleSystem particles;
	for (size_t i = 0; i <200; i++)
	{
		glm::vec2 offset(randomFloatBetween(-1, 1), randomFloatBetween(-1, 1));
		offset = glm::normalize(offset);
		glm::vec2 direction = offset;

		offset *= randomFloatBetween(0, 0.3);
		direction *= randomFloatBetween(0.5, 1);
		
		GLfloat lifeTime = randomFloatBetween(0.2, 0.7);
		
		
		GLubyte r = 200, g = 10, b = 20;
		randomizeColor(r, g, b, 30);
		particles.addParticle(x + offset.x, y + offset.y, direction.x, direction.y, lifeTime, r, g, b, 255);
	}
	addParticles(particles);
}

void ParticleSystem::mouseDragParticles(glm::vec2 mousePosition, glm::vec2 target, glm::vec4 color, double deltaTime)
{
	ParticleSystem particles;
	for (size_t i = 0; i <= 100 * deltaTime; i++) {
		glm::vec2 offset(randomFloatBetween(-1, 1), randomFloatBetween(-1, 1));
		offset = glm::normalize(offset) * randomFloatBetween(0.f, 0.2f);

		glm::vec2 position = offset + mousePosition;
		glm::vec2 velocity = target - position;
		float lifeTime = randomFloatBetween(0.3, 0.7);
		particles.addParticle(position, velocity, lifeTime, color);
	}
	addParticles(particles);
}

void ParticleSystem::spawnAcknowledgeParticles(glm::vec2 position)
{
	ParticleSystem particles;
	for (size_t i = 0; i <100; i++)
	{
		glm::vec2 offset(randomFloatBetween(-1, 1), randomFloatBetween(-1, 1));
		offset = glm::normalize(offset);
		glm::vec2 direction = offset;

		offset *= randomFloatBetween(0, 0.1);
		direction *= randomFloatBetween(0.5, 1);

		GLfloat lifeTime = randomFloatBetween(0.1, 0.5);

		particles.addParticle(offset + position, direction, lifeTime, glm::vec4(randomFloat(),randomFloat(), randomFloat(), 0.8));
	}
	addParticles(particles);
}

std::queue<ParticleSystem>& ParticleSystem::particlesToSpawn()
{
	return particleQueue;
}

std::queue<ParticleSystem> ParticleSystem::particleQueue;

float ParticleSystem::randomFloat()
{
	return (float) rand() / (float) RAND_MAX;
}

void ParticleSystem::randomizeColor(GLubyte& r, GLubyte& g, GLubyte b, GLubyte maximumDeviation)
{
	r = clamp(r + randomFloatBetween(-maximumDeviation, maximumDeviation), 0.f, 255.f);
	g = clamp(g + randomFloatBetween(-maximumDeviation, maximumDeviation), 0.f, 255.f);
	b = clamp(b + randomFloatBetween(-maximumDeviation, maximumDeviation), 0.f, 255.f);
}

float ParticleSystem::randomFloatBetween(float minimum, float maximum)
{
	return randomFloat() * (maximum - minimum) + minimum;
}

void ParticleSystem::addParticle(GLfloat x, GLfloat y, GLfloat xVelocity, GLfloat yVelocity, GLfloat lifetime, GLubyte r, GLubyte g, GLubyte b, GLubyte alpha)
{
	dynamicData.push_back(x);
	dynamicData.push_back(y);
	dynamicData.push_back(lifetime);
	dynamicData.push_back(0);

	staticData.push_back(xVelocity);
	staticData.push_back(yVelocity);
	staticData.push_back(0);
	staticData.push_back(0);

	color.push_back(r);
	color.push_back(g);
	color.push_back(b);
	color.push_back(alpha);
}

void ParticleSystem::addParticle(glm::vec2 position, glm::vec2 velocity, float lifeTime, glm::vec4 color)
{
	addParticle(position.x, position.y, velocity.x, velocity.y, lifeTime, color.x * 255, color.y * 255, color.z * 255, color.w * 255);
}

template<class T>
T ParticleSystem::clamp(T value, T minimum, T maximum)
{
	return std::min(maximum, std::max(minimum, value));
}
