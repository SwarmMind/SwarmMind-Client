#include <algorithm>

#include <gamestate/Gamestate.h>

using namespace std;

Gamestate::Gamestate(const EntityMap& _units, const EntityMap& _monsters)
	: units{_units}
	, monsters{_monsters}
{
}

Gamestate::~Gamestate()
{

}

EntityMap Gamestate::getUnits() const
{
	return units;
}

EntityMap Gamestate::getMonsters() const
{
	return monsters;
}
