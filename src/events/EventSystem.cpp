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
    std::vector<ListenerFunction*> finishedListeners;
    std::vector<ListenerFunction*>::iterator found;
    while ((found = std::find_if(listeners.begin(), listeners.end(), [=](ListenerFunction* listener) {
        return std::find(finishedListeners.begin(), finishedListeners.end(), listener) == finishedListeners.end();
    })) != listeners.end())
    {
        ListenerFunction* listener = *found;
        (*listener)(_event);
        finishedListeners.push_back(listener);
    }
}

