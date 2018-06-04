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

