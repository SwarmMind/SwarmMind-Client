#pragma once
#include <cstdint>
#include <glm/vec2.hpp>
class Gamestate;

class Command
{
public:
	virtual ~Command() = default;

	virtual void executeOn(Gamestate& state) = 0;
protected:
	Command(uint32_t _ID);
	
private:
	uint32_t ID;
};

class DirectionalCommand : public Command
{
public:
	virtual ~DirectionalCommand() = default;
protected:
	DirectionalCommand(uint32_t _ID, glm::vec2 _direction);
	
private:
	glm::vec2 direction;
};

class MoveCommand : public DirectionalCommand
{
public:
	MoveCommand(uint32_t _ID, glm::vec2 _direction);
	virtual ~MoveCommand() = default;
protected:
	
private:
};