#pragma once

#include <string>
#include <glm/glm.hpp>
#include <renderer/Sprites.h>
#include <nlohmann/json_fwd.hpp>


class Entity
{
public:
	virtual ~Entity() = default;

	glm::vec2 position() const;
	uint32_t id() const;
	uint32_t intid() const;
	bool operator<(const Entity& rhs) const;

	void moveTo(glm::vec2 position);

	void update(float deltaTime);
	void draw(class Renderer& renderer);
protected:
	Entity(std::uint32_t _id, glm::vec2 _position);
	virtual SpriteEnum sprite() = 0;


private:
	glm::vec2 currentPosition;
	float velocity = 2;
	glm::vec2 targetPos;
	
	std::uint32_t ID;
};

class Unit : public Entity
{
public:
	Unit(const nlohmann::json& json);
	Unit(std::uint32_t _id, glm::vec2 _position);
	virtual ~Unit();
protected:
	virtual SpriteEnum sprite() override;
private:
};

class Monster : public Entity
{
public:
	Monster(const nlohmann::json& json);
	Monster(std::uint32_t _id, glm::vec2 _position);
	virtual ~Monster();
protected:
	virtual SpriteEnum sprite() override;
private:
};