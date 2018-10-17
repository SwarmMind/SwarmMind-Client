#pragma once

#include <array>
#include <vector>
#include <glm/vec2.hpp>
#include <glbinding/gl41core/gl.h>
using namespace gl41core;

class CommandVisualizer
{
public:
    CommandVisualizer() = default;
	CommandVisualizer(GLubyte r, GLubyte g, GLubyte b, float _radius = 0.5);

	~CommandVisualizer();

	void setCommands(std::vector<glm::vec2> directions);
	
	static const unsigned int m_numVertices = 50;
	std::array<GLubyte, m_numVertices * 4> m_vertexColors;
	
	GLubyte m_baseRed, m_baseGreen, m_baseBlue;
	float m_radius;
private:
	size_t m_visualizationRadius = 4;
	float directionAngle(glm::vec2 direction);
	size_t lowerIndex(glm::vec2 direction);
	size_t higherIndex(glm::vec2 direction);
	size_t closestIndex(glm::vec2 direction);
	void addAlphaAtIndex(int index, GLuint alpha);

};
