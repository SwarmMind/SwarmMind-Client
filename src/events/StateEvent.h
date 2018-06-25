#pragma once

#include <events/Event.h>
#include <string>
#include <gamestate/Gamestate.h>
#include <memory>

class StateEvent : public Event
{
public:
    StateEvent() = default;

    virtual ~StateEvent() = default;

    Gamestate* state;

};