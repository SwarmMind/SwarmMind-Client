#include <gamestate/Command.h>

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
