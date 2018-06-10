#pragma once

#include <cstdint>

#include <map>
#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include <gamestate/Entity.h>

class Configuration 
{
public:
	uint32_t sizeX;
	uint32_t sizeY;
};

class Gamestate
{
public:
	Gamestate() {}
	Gamestate(const std::map<uint32_t, Unit>& _units, const std::map<uint32_t, Monster>& _monsters);
	~Gamestate();

	void update(double deltaTime);
	void draw(class Renderer& renderer);

	Entity* getEntityByID(uint32_t ID);

	std::map<uint32_t, Unit> units;
	std::map<uint32_t, Monster> monsters;
};
