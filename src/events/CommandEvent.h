#pragma once

#include <events/Event.h>
#include <string>
#include <gamestate/Command.h>
#include <memory>

class CommandEvent : public Event
{
public:
	CommandEvent() = default;
	CommandEvent(const std::shared_ptr<Command>& _command)
		: command{ _command }
	{}

	virtual ~CommandEvent() = default;

	std::shared_ptr<Command> command;
private:

};