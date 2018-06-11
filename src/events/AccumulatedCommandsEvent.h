#pragma once
#include <events/Event.h>

#include <vector>
#include <glm/vec2.hpp>

struct AccumulatedCommands {
	uint32_t ID;
	std::vector<glm::vec2> attackDirections;
	std::vector<glm::vec2> moveDirections;
};

class AccumulatedCommandsEvent : public Event
{
public:
	AccumulatedCommandsEvent() = default;
	virtual ~AccumulatedCommandsEvent() = default;

	std::vector<AccumulatedCommands> commands;
protected:
	
private:
};