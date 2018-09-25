#pragma once

#include <events/Event.h>
#include <string>
#include <gamestate/Gamestate.h>
#include <memory>

class InitStateEvent : public Event
{
public:
    InitStateEvent() = default;
	InitStateEvent(double timeSinceLastRound, Configuration config, std::shared_ptr<Gamestate> state)
		: m_timeSinceLastRound{timeSinceLastRound}
		, m_config{ config }
		, m_state{ state } 
	{}

    virtual ~InitStateEvent() = default;

    double m_timeSinceLastRound;
    Configuration m_config;
    std::shared_ptr<Gamestate> m_state;

};