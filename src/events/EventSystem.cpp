#include <events/EventSystem.h>
#include <algorithm>
#include <assert.h>

EventSystem::EventSystem()
{

}

EventSystem::~EventSystem()
{

}

void EventSystem::registerListener(ListenerFunction* listener)
{
	listeners.push_back(listener);
}

void EventSystem::removeListener(ListenerFunction* listener)
{
	auto iterator = std::find(listeners.begin(), listeners.end(), listener);
	if (iterator != listeners.end())
	{
		listeners.erase(iterator);
	}
}

void EventSystem::processEvent(Event* _event)
{
	for (ListenerFunction* listener : listeners)
	{
		(*listener)(_event);
	}
}

