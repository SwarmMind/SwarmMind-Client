#pragma once
#include <cstdint>
#include <glm/vec2.hpp>
#include <sound/Sound.h>

class Gamestate;

class Command
{
public:
	virtual ~Command() = default;

	virtual void executeOn(Gamestate& state) = 0;
protected:
	Command(uint32_t _ID, Sound& sound);
	uint32_t ID;
    class Sound& m_sound;
	
private:
};

class DieCommand : public Command
{
public:
	DieCommand(uint32_t _ID, Sound& sound);
	virtual ~DieCommand() = default;

	virtual void executeOn(Gamestate& state) override;
protected:
	
private:
};

class DirectionalCommand : public Command
{
public:
	virtual ~DirectionalCommand() = default;
protected:
	DirectionalCommand(uint32_t _ID, glm::vec2 _direction, Sound& sound);
	glm::vec2 direction;
};

class MoveCommand : public DirectionalCommand
{
public:
	MoveCommand(uint32_t _ID, glm::vec2 _direction, Sound& sound);
	virtual ~MoveCommand() = default;

	virtual void executeOn(Gamestate& state) override;
};

class AttackCommand : public DirectionalCommand
{
public:
	AttackCommand(uint32_t _ID, glm::vec2 _direction, Sound& sound);
	virtual ~AttackCommand() = default;

	virtual void executeOn(Gamestate& state) override;
};

class DamageCommand : public DirectionalCommand
{
public:
	DamageCommand(uint32_t _ID, glm::vec2 _direction, Sound& sound);
	virtual ~DamageCommand() = default;

	virtual void executeOn(Gamestate& state) override;
};