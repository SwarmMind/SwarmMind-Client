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
	auto iterator = std::find_if(listeners.begin(), listeners.end(), [=](const ListenerFunction* otherListener) {
		//this will only work if the std::function object is always passed per reference, so the pointers stay intact
		return listener == otherListener;
	});
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

