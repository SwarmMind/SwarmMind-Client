#pragma once

#include <events/Event.h>
#include <string>
#include <gamestate/Gamestate.h>
#include <memory>

class DisconnectEvent : public Event
{
public:
    DisconnectEvent() = default;

    virtual ~DisconnectEvent() = default;

};