#include <gamestate/Command.h>
#include <gamestate/Entity.h>
#include <gamestate/Gamestate.h>

Command::Command(uint32_t _ID)
	: ID{_ID}
{}

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
