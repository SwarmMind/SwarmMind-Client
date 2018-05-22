#pragma once

#include <vector>
#include <string>

struct Configuration 
{
	unsigned sizeX;
	unsigned sizeY;
};

struct Entity
{
	std::string id;
	unsigned posX;
	unsigned posY;
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
