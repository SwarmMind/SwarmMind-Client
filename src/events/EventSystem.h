#pragma once

#include <events/Event.h>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <vector>
#include <functional>


class EventSystem
{
public:
	using ListenerFunction = std::function<void(Event*)>;
	EventSystem();

	~EventSystem();

	void registerListener(ListenerFunction* listener);
	void removeListener(ListenerFunction* listener);
	void processEvent(Event* _event);
private:
	std::vector<ListenerFunction*> listeners;
};
