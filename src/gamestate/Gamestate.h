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

/*
struct Entity
{
	std::string id;
	uint32_t posX;
	uint32_t posY;
	glm::vec2 pos() const {
		return glm::vec2{ posX, posY };
	}
	bool is_npc() const {
		return id.substr(0, 3).compare("npc") == 0;
	}
	uint32_t intid() const {
		return std::stoi(id.substr(is_npc() ? 3 : 4));
	}
	bool operator<(const Entity& rhs) const {
		return intid() < rhs.intid() && is_npc() < rhs.is_npc();
	}
};*/

using EntityMap = std::map<uint32_t, Entity>;

class Gamestate
{
public:
	Gamestate() {}
	Gamestate(const EntityMap& _units, const EntityMap& _monsters);
	~Gamestate();

	EntityMap units, monsters;
};
