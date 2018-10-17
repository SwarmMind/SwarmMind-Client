#include <gamestate/Command.h>
#include <gamestate/Entity.h>
#include <gamestate/Gamestate.h>
#include <renderer/ParticleSystem.h>
#include <sound/Sounds.h>
#include <gamestate/Gamestate.h>
#include <gamestate/Map.h>
#include <stdlib.h>
#include <time.h>


Command::Command(uint32_t ID, CommandType type)
	: m_type{type}
	, m_ID{ID}
{}

CommandType Command::type() const {
	return m_type;
}

//////////////////////////////////////////////////////////////////////////
//						DieCommand
//////////////////////////////////////////////////////////////////////////
DieCommand::DieCommand(uint32_t _ID)
	: Command(_ID, CommandType::Die)
{}

void DieCommand::executeOn(Gamestate& state)
{
    state.deleteEntity(m_ID);
}

DirectionalCommand::DirectionalCommand(uint32_t _ID, CommandType _type, glm::vec2 _direction)
	: Command(_ID, _type)
	, m_direction{_direction}
{}

//////////////////////////////////////////////////////////////////////////	
//						 MoveCommand
//////////////////////////////////////////////////////////////////////////

MoveCommand::MoveCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand(_ID, CommandType::Move, _direction)
{}

void MoveCommand::executeOn(Gamestate& state)
{
    Entity* entity = state.getEntityByID(m_ID);
	if (!entity) return;

    entity->moveTo(entity->position() + m_direction);

	entity->rotate(m_direction);
}

//////////////////////////////////////////////////////////////////////////	
//						AttackCommand
//////////////////////////////////////////////////////////////////////////
AttackCommand::AttackCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand(_ID, CommandType::Attack, _direction)
{}

void AttackCommand::executeOn(Gamestate& state)
{
	Unit* unit = dynamic_cast<Unit*>(state.getEntityByID(m_ID));
	if (!unit) return;
	
    ParticleSystem::spawnShootParticles(unit->position() + glm::normalize(m_direction) * 0.3f, m_direction);

	unit->rotate(m_direction);
}

//////////////////////////////////////////////////////////////////////////
//						DamageCommand
//////////////////////////////////////////////////////////////////////////
DamageCommand::DamageCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand(_ID, CommandType::Damage, _direction)
{}

void DamageCommand::executeOn(Gamestate& state)
{
	Entity* entity = state.getEntityByID(m_ID);
	if (!entity) return;

    ParticleSystem::spawnBloodParticles(entity->position(), m_direction);
}



//////////////////////////////////////////////////////////////////////////
//                      SpawnCommand
//////////////////////////////////////////////////////////////////////////
SpawnCommand::SpawnCommand(uint32_t _ID, glm::vec2 position, bool isUnit)
    : Command(_ID, CommandType::Spawn)
    , m_position(position)
    , m_isUnit(isUnit)
{}

void SpawnCommand::executeOn(Gamestate& state)
{
    if (m_isUnit)
    {
        state.m_units.emplace(m_ID, Unit(m_ID, m_position));
    }
    else
    {
        state.m_monsters.emplace(m_ID, Monster(m_ID, m_position));
    }
}
