#pragma once
#include <events/Event.h>
#include <gamestate/ChatEntry.h>

class ChatEvent : public Event
{
public:
	ChatEvent() = default;
	ChatEvent(ChatEntry chatEntry)
		: m_chatEntry{ chatEntry }
	{}
	virtual ~ChatEvent() = default;

    ChatEntry m_chatEntry;
protected:
private:
};