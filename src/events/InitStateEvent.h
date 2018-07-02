#pragma once

#include <events/Event.h>
#include <string>
#include <gamestate/Gamestate.h>
#include <memory>

class InitStateEvent : public Event
{
public:
    InitStateEvent() = default;

    virtual ~InitStateEvent() = default;

    Configuration m_config;
    Gamestate* m_state;

};