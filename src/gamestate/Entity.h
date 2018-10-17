#pragma once

#include <string>
#include <glm/glm.hpp>
#include <renderer/Sprites.h>
#include <nlohmann/json_fwd.hpp>
#include <renderer/CommandVisualizer.h>


class Entity
{
public:
	virtual ~Entity() = default;

	glm::vec2 position() const;
	uint32_t id() const;
	bool operator<(const Entity& rhs) const;

	void moveTo(glm::vec2 position);
	void rotate(glm::vec2 direction);

    glm::vec2 rotation() const;

	void update(double deltaTime);
	virtual void draw(class Renderer& renderer);
protected:
    Entity() = default;
	Entity(std::uint32_t _id, glm::vec2 _position);
	virtual SpriteEnum sprite() = 0;


private:
	glm::vec2 m_currentPosition, m_targetPos, m_direction;
	const float m_velocity = 2;
	
	std::uint32_t m_id;
};

class Unit : public Entity
{
public:
    Unit() = default;
	Unit(const nlohmann::json& json);
	Unit(std::uint32_t _id, glm::vec2 _position);
	virtual ~Unit() = default;

	void setAttackCommands(std::vector<glm::vec2> directions);
	void setMoveCommands(std::vector<glm::vec2> directions);

	virtual void draw(Renderer& renderer) override;
protected:
	virtual SpriteEnum sprite() override;
private:
	CommandVisualizer m_attackVisualizer, m_moveVisualizer;
};

class Monster : public Entity
{
public:
    Monster() = default;
	Monster(const nlohmann::json& json);
	Monster(std::uint32_t _id, glm::vec2 _position);
	virtual ~Monster() = default;
protected:
	virtual SpriteEnum sprite() override;
private:
};