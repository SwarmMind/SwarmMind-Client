#include <algorithm>

#include <gamestate/Gamestate.h>
#include <events/EventSystem.h>

using namespace std;

Gamestate::Gamestate(class EventSystem& eventSystem, std::map<uint32_t, Unit>& _units, const std::map<uint32_t, Monster>& _monsters)
	: EventListener<CommandEvent>(eventSystem)
	, EventListener<AccumulatedCommandsEvent>(eventSystem)
	, units{ _units }
    , m_map{nullptr}
	, monsters{ _monsters }
{}

Gamestate::Gamestate(EventSystem& eventSystem)
	: EventListener<CommandEvent>(eventSystem)
	, EventListener<AccumulatedCommandsEvent>(eventSystem)
{}

Gamestate::~Gamestate()
{

}

void Gamestate::update(double deltaTime)
{
	for (auto& unit : units)
	{
		unit.second.update(deltaTime);
	}

	for (auto& monster : monsters)
	{
		monster.second.update(deltaTime);
	}
}

void Gamestate::setMap(Map* map) {
    m_map = map;
}

void Gamestate::draw(class Renderer& renderer)
{
	for (auto& unit : units)
	{
		unit.second.draw(renderer);
	}

	for (auto& monster : monsters)
	{
		monster.second.draw(renderer);
	}
}

Entity* Gamestate::getEntityByID(uint32_t ID)
{
	if (units.find(ID) != units.end())
	{
		return &units.at(ID);
	}
	if (monsters.find(ID) != monsters.end())
	{
		return &monsters.at(ID);
	}
	return nullptr;
}

void Gamestate::deleteEntity(uint32_t ID)
{
	units.erase(ID);
	monsters.erase(ID);
}

void Gamestate::receiveEvent(CommandEvent* event)
{
	event->command->executeOn(*this);
}

void Gamestate::receiveEvent(AccumulatedCommandsEvent* event)
{
	for (AccumulatedCommands commands : event->m_commands)
	{
		Unit* unit = dynamic_cast<Unit*>(getEntityByID(commands.ID));
		if (unit != nullptr)
		{
			unit->setAttackCommands(commands.attackDirections);
			unit->setMoveCommands(commands.moveDirections);
		}
	}
}
