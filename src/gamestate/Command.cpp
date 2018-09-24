#include <gamestate/Command.h>
#include <gamestate/Entity.h>
#include <gamestate/Gamestate.h>
#include <renderer/ParticleSystem.h>
#include <sound/Sounds.h>
#include <gamestate/Gamestate.h>
#include <gamestate/Map.h>
#include <stdlib.h>
#include <time.h>


Command::Command(uint32_t _ID, CommandType _type)
	: m_type{_type}
	, ID{_ID}
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
    //srand(time(NULL));
    //const char dieSound[5] = { 'SoundEnum::Die1', 'SoundEnum::Die2', 'SoundEnum::Die3', 'SoundEnum::Die4', 'SoundEnum::Die5' };
    //int randomDie = rand() % 5; 
    //state.m_map->getSounds().play(dieSound[randomDie]);
    //state.m_map->getSounds().play(SoundEnum::Die1);

    state.deleteEntity(ID);
}

DirectionalCommand::DirectionalCommand(uint32_t _ID, CommandType _type, glm::vec2 _direction)
	: Command(_ID, _type)
	, direction{_direction}
{}

//////////////////////////////////////////////////////////////////////////	
//						 MoveCommand
//////////////////////////////////////////////////////////////////////////

MoveCommand::MoveCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand(_ID, CommandType::Move, _direction)
{}

void MoveCommand::executeOn(Gamestate& state)
{
    Entity* entity = state.getEntityByID(ID);

	if (!entity) return;

    //state.m_map->getSounds().play(SoundEnum::Walk); 
    entity->moveTo(entity->position() + direction);

}

//////////////////////////////////////////////////////////////////////////	
//						AttackCommand
//////////////////////////////////////////////////////////////////////////
AttackCommand::AttackCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand(_ID, CommandType::Attack, _direction)
{}

void AttackCommand::executeOn(Gamestate& state)
{
	Unit* unit = dynamic_cast<Unit*>(state.getEntityByID(ID));

    //srand(time(NULL));
    //const char attackSound[3] = { 'SoundEnum::Attack1', 'SoundEnum::Attack2', 'SoundEnum::Attack3' };
    //int randomAttack = rand() % 3;

	if (!unit) return;
	
    //state.m_map->getSounds().play(attackSound[randomAttack]); 
    //state.m_map->getSounds().play(SoundEnum::Attack1); 
    ParticleSystem::spawnShootParticles(unit->position() + glm::normalize(direction) * 0.3f, direction);
	
}

//////////////////////////////////////////////////////////////////////////
//						DamageCommand
//////////////////////////////////////////////////////////////////////////
DamageCommand::DamageCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand(_ID, CommandType::Damage, _direction)
{}

void DamageCommand::executeOn(Gamestate& state)
{
	Entity* entity = state.getEntityByID(ID);

    //srand(time(NULL));
    //const char hitSound[5] = { 'SoundEnum::Hit1', 'SoundEnum::Hit2', 'SoundEnum::Hit3', 'SoundEnum::Hit4', 'SoundEnum::Hit5' };
    //int randomSound = rand() % 5;

	if (!entity) return;

    //state.m_map->getSounds().play(hitSound[randomSound]);
    //state.m_map->getSounds().play(SoundEnum::Hit1);
    ParticleSystem::spawnBloodParticles(entity->position(), direction);

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
        state.units.emplace(ID, Unit(ID, m_position));
    }
    else
    {
        state.monsters.emplace(ID, Monster(ID, m_position));
    }
}
