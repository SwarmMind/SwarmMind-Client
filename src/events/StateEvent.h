#pragma once

#include <events/Event.h>
#include <string>
#include <memory>

class StateEvent : public Event
{
public:
    StateEvent() 
        : m_state{nullptr}
    {}

    virtual ~StateEvent()
    {
        delete m_state;
    }

    class Gamestate* m_state;
};