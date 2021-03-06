#include <renderer/ParticleSystem.h>
#include <random>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>

static std::mt19937_64 random = std::mt19937_64{ std::random_device{}() };

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::addParticles(Particles particles)
{
	particleQueue.push(particles);
}

glm::vec2 ParticleSystem::randomOffset(const float maximum) {
	const float r = randomFloatBetween(0.0f, maximum), phi = randomFloatBetween(0.0f, glm::pi<float>() * 2.0f);
	return glm::vec2{ r * cos(phi), r * sin(phi) };
}

void ParticleSystem::spawnTestParticles(float x, float y, float width, float height)
{
	auto randomColor = [&] { return static_cast<GLubyte>(std::uniform_int_distribution<unsigned int>{ 0, 255 }(random)); };

	Particles particles;
	for (size_t i = 0; i < 100; i++)
	{
		GLfloat directionX = randomFloatBetween(-width, width), directionY = randomFloatBetween(-height, height);
		GLfloat lifeTime = randomFloatBetween(0.5, 1.5);
		GLubyte r = randomColor(), g = randomColor(), b = randomColor(), alpha = 255;

		particles.addParticle(x, y, directionX, directionY, lifeTime, r, g, b, alpha);
	}
	addParticles(particles);
}

void ParticleSystem::spawnBloodParticles(glm::vec2 position, glm::vec2 direction)
{
	direction = glm::normalize(direction);
	Particles particles;
	for (size_t i = 0; i < 200; i++)
	{
		const glm::vec2 offset{ randomOffset(0.3f) };
		const glm::vec2 particleDirection = randomFloatBetween(0.5f, 1.0f) * (glm::normalize(offset) + direction * randomFloatBetween(1.f, 2.f));

		GLfloat lifeTime = randomFloatBetween(0.2f, 0.7f);

		const glm::vec4 color = randomizeColor(glm::vec4{ 0.8f, 0.04f, 0.08f, 1.0f }, 0.1f);
		particles.addParticle(position + offset, particleDirection, lifeTime, color);
	}
	addParticles(particles);
}

void ParticleSystem::spawnShootParticles(glm::vec2 position, glm::vec2 direction)
{
	direction = glm::normalize(direction);
	Particles particles;

	//Muzzle flash
	for (size_t i = 0; i < 200; i++)
	{
		float angle = randomFloatBetween(-10.0f, 10.0f);
		glm::vec2 randomizedDirection = glm::rotate(direction, glm::radians(angle));
		glm::vec2 velocity = randomizedDirection * randomFloatBetween(0.2f, 1.f);
		glm::vec2 offset = randomOffset(0.1f) + randomizedDirection * randomFloatBetween(0.1f, 0.3f);

		GLfloat lifeTime = randomFloatBetween(0.2f, 0.7f);

		const glm::vec4 color = randomizeColor(glm::vec4{ 1.f, 0.64f, 0.f, 0.5f }, 0.2f, true);
		particles.addParticle(position + offset, velocity, lifeTime, color);
	}

    //Smoke
    for (size_t i = 0; i < 200; i++)
    {
        float angle = randomFloatBetween(-50.0f, 50.0f);
        glm::vec2 randomizedDirection = glm::rotate(direction, glm::radians(angle));
        glm::vec2 velocity = randomizedDirection * randomFloatBetween(0.1f, 0.5f);
        glm::vec2 offset = randomOffset(0.05f) + randomizedDirection * randomFloatBetween(0.f, 0.2f);

        GLfloat lifeTime = randomFloatBetween(0.5f, 1.2f);

        const glm::vec4 color = randomizeColor(glm::vec4{ 0.6f, 0.6f, 0.6f, 0.5f }, 0.05f, true);
        particles.addParticle(position + offset, velocity, lifeTime, color);
    }

	addParticles(particles);
}

void ParticleSystem::mouseDragParticles(glm::vec2 mousePosition, glm::vec2 target, glm::vec4 color, double deltaTime)
{
	Particles particles;
	for (size_t i = 0; i <= 100 * deltaTime; i++) {
		glm::vec2 offset{ randomOffset(0.2f) };

		glm::vec2 position = offset + mousePosition;
		glm::vec2 velocity = target - position;
		float lifeTime = randomFloatBetween(0.3f, 0.7f);
		particles.addParticle(position, velocity, lifeTime, color);
	}
	addParticles(particles);
}

void ParticleSystem::spawnAcknowledgeParticles(glm::vec2 position)
{
	Particles particles;
	for (size_t i = 0; i < 100; i++)
	{
		const glm::vec2 offset{ randomOffset(0.1f) };
		const glm::vec2 direction = randomFloatBetween(0.5f, 1.0f) * glm::normalize(offset);

		GLfloat lifeTime = randomFloatBetween(0.1f, 0.5f);

		particles.addParticle(offset + position, direction, lifeTime, glm::vec4(1.0f, 0.8f, 0.f, 0.8f));
	}
	addParticles(particles);
}

std::queue<Particles>& ParticleSystem::particlesToSpawn()
{
	return particleQueue;
}

std::queue<Particles> ParticleSystem::particleQueue;

glm::vec4 ParticleSystem::randomizeColor(glm::vec4 color, float maximumDeviation, bool randomizeAlpha /*= false*/)
{
	using dist = std::uniform_real_distribution<float>;
	auto around = [&](float c) { return dist{ std::max(0.0f, c - maximumDeviation), std::min(1.0f, c + maximumDeviation) }(random); };

	return glm::vec4(around(color.r), around(color.g), around(color.b), randomizeAlpha ? around(color.a) : color.a);
}

float ParticleSystem::randomFloatBetween(float minimum, float maximum)
{
	return std::uniform_real_distribution<float>{ minimum, maximum }(random);
}

void Particles::addParticle(GLfloat x, GLfloat y, GLfloat xVelocity, GLfloat yVelocity, GLfloat lifetime, GLubyte r, GLubyte g, GLubyte b, GLubyte alpha)
{
	m_dynamicData.push_back(x);
	m_dynamicData.push_back(y);
	m_dynamicData.push_back(lifetime);
	m_dynamicData.push_back(0.0f);

	m_staticData.push_back(xVelocity);
	m_staticData.push_back(yVelocity);
	m_staticData.push_back(0.0f);
	m_staticData.push_back(0);

	m_color.push_back(r);
	m_color.push_back(g);
	m_color.push_back(b);
	m_color.push_back(alpha);
}

void Particles::addParticle(glm::vec2 position, glm::vec2 velocity, float lifeTime, glm::vec4 color)
{
	const auto as_byte = [](float x) { return static_cast<GLubyte>(x * 255.0f); };
	addParticle(
		position.x, position.y,
		velocity.x, velocity.y,
		lifeTime,
		as_byte(color.x), as_byte(color.y), as_byte(color.z), as_byte(color.w));
}
