#pragma once
#include <events/Event.h>
#include <gamestate/ChatEntry.h>

class ChatEvent : public Event
{
public:
	ChatEvent() = default;
	virtual ~ChatEvent() = default;

    ChatEntry m_chatEntry;
protected:
private:
};