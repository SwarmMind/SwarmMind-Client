#pragma once
#include <events/Event.h>

#include <vector>
#include <glm/vec2.hpp>

struct AccumulatedCommands {
	uint32_t m_ID;
	std::vector<glm::vec2> m_attackDirections;
    std::vector<glm::vec2> m_moveDirections;
};

class AccumulatedCommandsEvent : public Event
{
public:
	AccumulatedCommandsEvent() = default;
	AccumulatedCommandsEvent(std::vector<AccumulatedCommands> commands, size_t numberOfGivenCommands, size_t maxNumberOfCommands)
		: m_commands{ commands }
		, m_numberOfGivenCommands{ numberOfGivenCommands }
		, m_maxNumberOfCommands{ maxNumberOfCommands }
	{}
	virtual ~AccumulatedCommandsEvent() = default;

	std::vector<AccumulatedCommands> m_commands;

    size_t m_numberOfGivenCommands = 0;
    size_t m_maxNumberOfCommands = 1;
protected:
	
private:
};