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
    Sound* sound = new Sound(m_soundBuffer, m_soundInstances);
    sound->load("play_and_reload.wav");
    sound->play();
    state.deleteEntity(ID);
    sound->update();
}

DirectionalCommand::DirectionalCommand(uint32_t _ID, glm::vec2 _direction)
	: Command(_ID)
	, direction{_direction}
{}

//////////////////////////////////////////////////////////////////////////	
//						 MoveCommand
//////////////////////////////////////////////////////////////////////////

MoveCommand::MoveCommand(uint32_t _ID, glm::vec2 _direction)
	: DirectionalCommand{_ID, _direction}
{}

void MoveCommand::executeOn(Gamestate& state)
{
    Sound* sound = new Sound(m_soundBuffer, m_soundInstances);
    sound->load("unit_walking.wav");

    Entity* entity = state.getEntityByID(ID);

	if (entity != nullptr)
	{
		entity->moveTo(entity->position() + direction);
        sound->play();
	}
    sound->update();
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
