#pragma once

#include <cstdint>

#include <vector>
#include <string>

#include <glm/vec2.hpp>

struct Configuration 
{
	uint32_t sizeX;
	uint32_t sizeY;
};

struct Entity
{
	std::string id;
	uint32_t posX;
	uint32_t posY;
	glm::vec2 pos() const {
		return glm::vec2{ posX, posY };
	}
};

class Gamestate
{
public:
	Gamestate(std::vector<Entity> _units, std::vector<Entity> _monsters);
	~Gamestate();

	
	std::vector<Entity> getUnits() const;
	std::vector<Entity> getMonsters() const;
private:
	std::vector<Entity> units;
	std::vector<Entity> monsters;
};
