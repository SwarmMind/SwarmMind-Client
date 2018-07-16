#include <gamestate/Command.h>
#include <gamestate/Entity.h>
#include <gamestate/Gamestate.h>
#include <renderer/ParticleSystem.h>

Command::Command(uint32_t _ID, Sound& sound)
	: ID{_ID}
    , m_sound{ sound }
{}

//////////////////////////////////////////////////////////////////////////
//						DieCommand
//////////////////////////////////////////////////////////////////////////
DieCommand::DieCommand(uint32_t _ID, Sound& sound)
	: Command( _ID, sound )
{}

void DieCommand::executeOn(Gamestate& state)
{
    m_sound.load("play_and_reload.wav");
    m_sound.play();
    state.deleteEntity(ID);
}

DirectionalCommand::DirectionalCommand(uint32_t _ID, glm::vec2 _direction, Sound& sound)
	: Command(_ID, sound)
	, direction{_direction}
{}

//////////////////////////////////////////////////////////////////////////	
//						 MoveCommand
//////////////////////////////////////////////////////////////////////////

MoveCommand::MoveCommand(uint32_t _ID, glm::vec2 _direction, Sound& sound)
	: DirectionalCommand(_ID, _direction, sound)
{}

void MoveCommand::executeOn(Gamestate& state)
{
    m_sound.load("unit_walking.wav");

    Entity* entity = state.getEntityByID(ID);

	if (entity != nullptr)
	{
		entity->moveTo(entity->position() + direction);
        m_sound.play();
	}
}

//////////////////////////////////////////////////////////////////////////	
//						AttackCommand
//////////////////////////////////////////////////////////////////////////
AttackCommand::AttackCommand(uint32_t _ID, glm::vec2 _direction, Sound& sound)
	: DirectionalCommand(_ID, _direction, sound)
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
DamageCommand::DamageCommand(uint32_t _ID, glm::vec2 _direction, Sound& sound)
	: DirectionalCommand(_ID, _direction, sound)
{}

void DamageCommand::executeOn(Gamestate& state)
{
	Entity* entity = state.getEntityByID(ID);
	if (entity != nullptr)
	{
		ParticleSystem::spawnBloodParticles(entity->position(), direction);
        m_sound.load("play_and_reload.wav");
        m_sound.play();
	}
}
