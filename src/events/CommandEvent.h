#pragma once

#include <events/Event.h>
#include <string>

class CommandEvent : public Event
{
public:
	CommandEvent() = default;

	virtual ~CommandEvent() = default;

	std::string myString;
private:

};