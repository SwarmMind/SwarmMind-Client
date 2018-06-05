#include <renderer/CommandVisualizer.h>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

const unsigned int CommandVisualizer::numVertices;

CommandVisualizer::CommandVisualizer(GLubyte r, GLubyte g, GLubyte b, float _radius /*= 0.5*/)
	: baseRed { r }
	, baseGreen { g }
	, baseBlue { b }
	, radius { _radius }
{
	for (size_t i = 0; i < numVertices; i++)
	{
		vertexColors.at(i * 4) = r;
		vertexColors.at(i * 4 + 1) = g;
		vertexColors.at(i * 4 + 2) = b;
		vertexColors.at(i * 4 + 3) = 0;
	}
}

CommandVisualizer::~CommandVisualizer()
{

}

void CommandVisualizer::setCommands(std::vector<glm::vec2> directions)
{
	//Reset all alpha values
	for (size_t i = 0; i < numVertices; i++)
	{
		vertexColors.at(i * 4 + 3) = 0;
	}

	GLubyte alpha = 255 / directions.size();
	for (auto& direction : directions)
	{
		direction = glm::normalize(direction);
		size_t index = closestIndex(direction);
		addAlphaAtIndex(index, alpha);
		for (size_t i = 1; i < visualizationRadius; i++)
		{
			addAlphaAtIndex(index + i, alpha);
			addAlphaAtIndex(index - i, alpha);
		}
	}

	//First and last vertex must have the same values
	vertexColors.at(vertexColors.size() - 4) = vertexColors.at(0);
	vertexColors.at(vertexColors.size() - 3) = vertexColors.at(1);
	vertexColors.at(vertexColors.size() - 2) = vertexColors.at(2);
	vertexColors.at(vertexColors.size() - 1) = vertexColors.at(3);
}

float CommandVisualizer::directionAngle(glm::vec2 direction)
{
	glm::vec2 baseDirection(-1, 0);
	float angle = glm::degrees(std::atan2(direction.y, direction.x) - std::atan2(baseDirection.y, baseDirection.x));
	if (angle < 0)
	{
		angle += 360.f;
	}
	while (angle >= 360.f)
	{
		angle -= 360.f;
	}
	return angle;
}

size_t CommandVisualizer::lowerIndex(glm::vec2 direction)
{
	float degreesPerVertex = 360.f / (CommandVisualizer::numVertices - 1);
	size_t index = static_cast<size_t>(std::floor(directionAngle(direction) / degreesPerVertex)) % (CommandVisualizer::numVertices - 1);
	return index;
}

size_t CommandVisualizer::higherIndex(glm::vec2 direction)
{
	float degreesPerVertex = 360.f / (CommandVisualizer::numVertices - 1);
	size_t index = static_cast<size_t>(std::ceil(directionAngle(direction) / degreesPerVertex)) % (CommandVisualizer::numVertices - 1);
	return index;
}

size_t CommandVisualizer::closestIndex(glm::vec2 direction)
{
	float degreesPerVertex = 360.f / (CommandVisualizer::numVertices - 1);
	size_t index = static_cast<size_t>(std::round(directionAngle(direction) / degreesPerVertex)) % (CommandVisualizer::numVertices - 1);
	return index;
}

void CommandVisualizer::addAlphaAtIndex(int index, GLuint alpha)
{
	while (index < 0)
	{
		index += CommandVisualizer::numVertices - 1;
	}

	index = index % (CommandVisualizer::numVertices - 1);
	GLubyte& vertexAlpha = vertexColors.at(index * 4 + 3);
	//Check for overflow
	if (vertexAlpha + alpha < vertexAlpha)
	{
		vertexAlpha = 255;
	}
	else 
	{
		vertexAlpha += alpha;
	}
}
