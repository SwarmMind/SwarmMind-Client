#pragma once

#include <string>
#include <glm/glm.hpp>

class Entity
{
public:
	Entity();

	~Entity();

	std::uint32_t id;
	
	glm::vec2 oldPos;
	glm::vec2 targetPos;

	glm::vec2 pos(double timeSinceLastUpdate) const;
	uint32_t intid() const;
	bool operator<(const Entity& rhs) const;

private:

};
