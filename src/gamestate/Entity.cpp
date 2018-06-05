#include <gamestate/Entity.h>


Entity::Entity()
{

}

Entity::~Entity()
{

}

glm::vec2 Entity::pos(double timeSinceLastUpdate) const
{
	return glm::mix(oldPos, targetPos, glm::smoothstep(0.0, 1.5, timeSinceLastUpdate));
}

uint32_t Entity::intid() const
{
	return id;
}

bool Entity::operator<(const Entity& rhs) const
{
	return intid() < rhs.intid();
}
