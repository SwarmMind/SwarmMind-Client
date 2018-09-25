#pragma once

#include <events/Event.h>
#include <string>
#include <memory>

class StateEvent : public Event
{
public:
    StateEvent()
    {}
	StateEvent(std::shared_ptr<Gamestate> state)
		: m_state(state)
	{}

    virtual ~StateEvent()
    {}

    std::shared_ptr<Gamestate> m_state;
};