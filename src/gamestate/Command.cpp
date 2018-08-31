#include <gamestate/Command.h>
#include <gamestate/Entity.h>
#include <gamestate/Gamestate.h>
#include <renderer/ParticleSystem.h>

Command::Command(uint32_t _ID)
	: ID{_ID}
{}

//////////////////////////////////////////////////////////////////////////
//						DieCommand
//////////////////////////////////////////////////////////////////////////
DieCommand::DieCommand(uint32_t _ID)
	: Command(_ID)
{}

void DieCommand::executeOn(Gamestate& state)
{
	state.deleteEntity(ID);
}

DirectionalCommand::DirectionalCommand(uint32_t _ID, glm::vec2 _direction)
	: Command(_ID)
	, direction{_direction}
{}

MoveCommand::MoveCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand{_ID, _direction}
{}

void MoveCommand::executeOn(Gamestate& state)
{
	Entity* entity = state.getEntityByID(ID);
	if (entity != nullptr)
	{
		entity->moveTo(entity->position() + direction);
	}
}




//////////////////////////////////////////////////////////////////////////	
//						AttackCommand
//////////////////////////////////////////////////////////////////////////
AttackCommand::AttackCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand(_ID, _direction)
{}

void AttackCommand::executeOn(Gamestate& state)
{
	Unit* unit = dynamic_cast<Unit*>(state.getEntityByID(ID));
	if (unit != nullptr)
	{
		ParticleSystem::spawnShootParticles(unit->position() + glm::normalize(direction) * 0.3f, direction);
	}
}



//////////////////////////////////////////////////////////////////////////
//						DamageCommand
//////////////////////////////////////////////////////////////////////////
DamageCommand::DamageCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand(_ID, _direction)
{}

void DamageCommand::executeOn(Gamestate& state)
{
	Entity* entity = state.getEntityByID(ID);
	if (entity != nullptr)
	{
		ParticleSystem::spawnBloodParticles(entity->position(), direction);
	}
}
