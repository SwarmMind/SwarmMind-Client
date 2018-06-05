#pragma once

#include <array>
#include <vector>
#include <glm/vec4.hpp>
#include <glbinding/gl41core/gl.h>
using namespace gl41core;

class CommandVisualizer
{
public:
	CommandVisualizer(GLubyte r, GLubyte g, GLubyte b, float _radius = 0.5);

	~CommandVisualizer();

	void setCommands(std::vector<glm::vec2> directions);
	
	static const unsigned int numVertices = 25;
	std::array<GLubyte, numVertices * 4> vertexColors;
	
	GLubyte baseRed, baseGreen, baseBlue;
	float radius;
private:
	size_t visualizationRadius = 2;
	float directionAngle(glm::vec2 direction);
	size_t lowerIndex(glm::vec2 direction);
	size_t higherIndex(glm::vec2 direction);
	size_t closestIndex(glm::vec2 direction);
	void addAlphaAtIndex(int index, GLuint alpha);

};
