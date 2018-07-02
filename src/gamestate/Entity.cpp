#include <gamestate/Entity.h>
#include <renderer/Renderer.h>
#include <nlohmann/json.hpp>

Entity::Entity(std::uint32_t _id, glm::vec2 _position)
	: ID{_id}
	, currentPosition{_position}
	, targetPos{_position}
{}


glm::vec2 Entity::position() const
{
	return currentPosition;
}

uint32_t Entity::id() const
{
	return ID;
}

uint32_t Entity::intid() const
{
	return ID;
}

bool Entity::operator<(const Entity& rhs) const
{
	return intid() < rhs.intid();
}

void Entity::moveTo(glm::vec2 position)
{
	targetPos = position;
}

void Entity::update(float deltaTime)
{
	glm::vec2 difference = targetPos - currentPosition;
	float distanceToMove = velocity * deltaTime;
	if (glm::length(difference) <= distanceToMove)
	{
		currentPosition = targetPos;
	}
	else 
	{
		currentPosition += glm::normalize(difference) * distanceToMove;
	}
}

void Entity::draw(class Renderer& renderer)
{
	renderer.drawSprite(glm::vec3(position() - glm::vec2(0.5f, 0.5f), 1), 1, 1, sprite());
}



//////////////////////////////////////////////////////////////////////////
//							Unit
//////////////////////////////////////////////////////////////////////////
Unit::Unit(std::uint32_t _id, glm::vec2 _position)
	: Entity(_id, _position)
	, attackVisualizer(255, 30, 30)
	, moveVisualizer(30, 255, 30, 0.7)
{}

Unit::Unit(const nlohmann::json& json)
	: Unit(json["ID"], glm::vec2(json["x"], json["y"]))
{}

void Unit::setAttackCommands(std::vector<glm::vec2> directions)
{
	attackVisualizer.setCommands(directions);
}

void Unit::setMoveCommands(std::vector<glm::vec2> directions)
{
	moveVisualizer.setCommands(directions);
}

void Unit::draw(Renderer& renderer)
{
	Entity::draw(renderer);

	renderer.drawCommandVisualizer(glm::vec3(position(), 1.f), attackVisualizer);
	renderer.drawCommandVisualizer(glm::vec3(position(), 1.f), moveVisualizer);
}

SpriteEnum Unit::sprite()
{
	return SpriteEnum::Unit;
}







//////////////////////////////////////////////////////////////////////////
//						Monster
//////////////////////////////////////////////////////////////////////////
Monster::Monster(std::uint32_t _id, glm::vec2 _position)
	: Entity{_id, _position}
{}

Monster::Monster(const nlohmann::json& json)
	: Monster(json["ID"], glm::vec2(json["x"], json["y"]))
{

}

SpriteEnum Monster::sprite()
{
	return SpriteEnum::Monster;
}