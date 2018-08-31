#pragma once

#include <events/Event.h>
#include <string>
#include <gamestate/Command.h>
#include <memory>

class CommandEvent : public Event
{
public:
	CommandEvent() = default;

	virtual ~CommandEvent() = default;

	std::shared_ptr<Command> command;
private:

};