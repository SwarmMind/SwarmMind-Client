#include <renderer/ParticleSystem.h>
#include <random>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
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
	for (size_t i = 0; i < 100; i++)
	{
		GLfloat directionX = randomFloatBetween(-width, width), directionY = randomFloatBetween(-height, height);
		GLfloat lifeTime = randomFloatBetween(0.5, 1.5);
		GLubyte r = randomFloat() * 255, g = randomFloat() * 255, b = randomFloat() * 255, alpha = 255;

		particles.addParticle(x, y, directionX, directionY, lifeTime, r, g, b, alpha);
	}
	addParticles(particles);
}

void ParticleSystem::spawnBloodParticles(glm::vec2 position, glm::vec2 direction)
{
	direction = glm::normalize(direction);
	ParticleSystem particles;
	for (size_t i = 0; i < 200; i++)
	{
		glm::vec2 offset(randomFloatBetween(-1, 1), randomFloatBetween(-1, 1));
		offset = glm::normalize(offset);
		glm::vec2 particleDirection = offset + direction * randomFloatBetween(1.f, 2.f);

		offset *= randomFloatBetween(0, 0.3);
		particleDirection *= randomFloatBetween(0.5, 1);

		GLfloat lifeTime = randomFloatBetween(0.2, 0.7);

		GLubyte r = 200, g = 10, b = 20;
		randomizeColor(r, g, b, 30);
		particles.addParticle(position + offset, particleDirection, lifeTime, glm::vec4((float)r / 255, (float)g/255, (float)b/255, 1.f));
	}
	addParticles(particles);
}

void ParticleSystem::spawnShootParticles(glm::vec2 position, glm::vec2 direction)
{
	direction = glm::normalize(direction);
	ParticleSystem particles;

	//Bullet
	float bulletSpeed = 20;
	float bulletLifeTime = 1;
	glm::vec4 bulletColor(0.1f, 0.1f, 0.1f, 1.0f);
	particles.addParticle(position, direction * bulletSpeed, bulletLifeTime, bulletColor);

	//Muzzle flash
	for (size_t i = 0; i < 200; i++)
	{
		float angle = randomFloatBetween(-10, 10);
		glm::vec2 randomizedDirection = glm::rotate(direction, glm::radians(angle));
		glm::vec2 velocity = randomizedDirection * randomFloatBetween(0.2f, 1.f);
		glm::vec2 offset = glm::normalize(glm::vec2(randomFloatBetween(-1.f, -1.f), randomFloatBetween(-1.f, 1.f))) * randomFloatBetween(0, 0.2f);
		offset += randomizedDirection * randomFloatBetween(0.f, 0.3f);

		GLfloat lifeTime = randomFloatBetween(0.2, 0.7);

		glm::vec4 color(1.f, 0.64f, 0.f, 0.5f);
		color = randomizeColor(color, 0.2f, true);
		particles.addParticle(position + offset, velocity, lifeTime, color);
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

glm::vec4 ParticleSystem::randomizeColor(glm::vec4 color, float maximumDeviation, bool randomizeAlpha /*= false*/)
{
	return glm::vec4(
		clamp(color.r + randomFloatBetween(-maximumDeviation, maximumDeviation), 0.f, 1.f),
		clamp(color.g + randomFloatBetween(-maximumDeviation, maximumDeviation), 0.f, 1.f),
		clamp(color.b + randomFloatBetween(-maximumDeviation, maximumDeviation), 0.f, 1.f),
		randomizeAlpha ?
			clamp(color.a + randomFloatBetween(-maximumDeviation, maximumDeviation), 0.f, 1.f) :
			color.a
	);
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
