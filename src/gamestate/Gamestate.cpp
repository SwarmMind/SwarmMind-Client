#include <gamestate/Gamestate.h>

using namespace std;

Gamestate::Gamestate(vector<Entity> _units, vector<Entity> _monsters)
	: units{_units}
	, monsters{_monsters}
{}

Gamestate::~Gamestate()
{

}

std::vector<Entity> Gamestate::getUnits() const
{
	return units;
}

std::vector<Entity> Gamestate::getMonsters() const
{
	return monsters;
}
