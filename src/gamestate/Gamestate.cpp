#include <algorithm>

#include <gamestate/Gamestate.h>
#include <events/EventSystem.h>

using namespace std;

Gamestate::Gamestate(class EventSystem& eventSystem, std::map<uint32_t, Unit>& units, const std::map<uint32_t, Monster>& monsters)
	: EventListener<CommandEvent>(eventSystem)
	, EventListener<AccumulatedCommandsEvent>(eventSystem)
	, m_units{ units }
    , m_map{nullptr}
	, m_monsters{ monsters }
{}

Gamestate::Gamestate(EventSystem& eventSystem)
	: EventListener<CommandEvent>(eventSystem)
	, EventListener<AccumulatedCommandsEvent>(eventSystem)
{}

Gamestate::~Gamestate()
{

}

void Gamestate::copyRotationsFrom(Gamestate& other)
{
    for (auto& unit : other.m_units)
    {
        if (m_units.find(unit.first) != m_units.end())
        {
            m_units[unit.first].rotate(unit.second.rotation());
        }
    }

    for (auto& monster : other.m_monsters)
    {
        if (m_monsters.find(monster.first) != m_monsters.end())
        {
            m_monsters[monster.first].rotate(monster.second.rotation());
        }
    }
}

void Gamestate::update(double deltaTime)
{
	for (auto& unit : m_units)
	{
		unit.second.update(deltaTime);
	}

	for (auto& monster : m_monsters)
	{
		monster.second.update(deltaTime);
	}
}

void Gamestate::setMap(Map* map) {
    m_map = map;
}

void Gamestate::draw(class Renderer& renderer)
{
	for (auto& unit : m_units)
	{
		unit.second.draw(renderer);
	}

	for (auto& monster : m_monsters)
	{
		monster.second.draw(renderer);
	}
}

Entity* Gamestate::getEntityByID(uint32_t ID)
{
	if (m_units.find(ID) != m_units.end())
	{
		return &m_units.at(ID);
	}
	if (m_monsters.find(ID) != m_monsters.end())
	{
		return &m_monsters.at(ID);
	}
	return nullptr;
}

void Gamestate::deleteEntity(uint32_t ID)
{
	m_units.erase(ID);
	m_monsters.erase(ID);
}

void Gamestate::receiveEvent(CommandEvent* event)
{
	event->m_command->executeOn(*this);
}

void Gamestate::receiveEvent(AccumulatedCommandsEvent* event)
{
	for (AccumulatedCommands commands : event->m_commands)
	{
		Unit* unit = dynamic_cast<Unit*>(getEntityByID(commands.m_ID));
		if (unit != nullptr)
		{
			unit->setAttackCommands(commands.m_attackDirections);
			unit->setMoveCommands(commands.m_moveDirections);
		}
	}
}
