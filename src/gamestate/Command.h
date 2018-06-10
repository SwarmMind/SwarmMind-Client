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
	uint32_t ID;
	
private:
};

class DirectionalCommand : public Command
{
public:
	virtual ~DirectionalCommand() = default;
protected:
	DirectionalCommand(uint32_t _ID, glm::vec2 _direction);
	glm::vec2 direction;
	
private:
};

class MoveCommand : public DirectionalCommand
{
public:
	MoveCommand(uint32_t _ID, glm::vec2 _direction);
	virtual ~MoveCommand() = default;

	virtual void executeOn(Gamestate& state) override;
protected:
	
private:
};