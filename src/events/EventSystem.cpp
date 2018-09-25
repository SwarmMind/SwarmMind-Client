#include <algorithm>
#include <mutex>
#include <cassert>
#include <functional>

#include <events/EventSystem.h>

EventSystem::EventSystem()
{

}

EventSystem::~EventSystem()
{

}

void EventSystem::registerListener(ListenerFunction* listener)
{
	listeners.emplace_back(listener);
}

void EventSystem::removeListener(ListenerFunction* listener)
{
	auto iterator = std::find(listeners.begin(), listeners.end(), listener);
	if (iterator != listeners.end())
	{
		listeners.erase(iterator);
	}
}

void EventSystem::postEvent(const std::shared_ptr<Event> event) {
	postEvent(TimedEvent{ event });
}

void EventSystem::postEvent(const TimedEvent& event)
{
	std::lock_guard<std::mutex> queueGuard{ queueLock };
	eventQueue.emplace(event);
}

void EventSystem::postEvents(const std::vector<TimedEvent>& events)
{
	std::lock_guard<std::mutex> queueGuard{ queueLock };
	std::for_each(events.cbegin(), events.cend(), [&](const auto& event) { eventQueue.push(event); });
}

void EventSystem::update(double deltaTime, double timeStamp)
{
	std::lock_guard<std::mutex> queueGuard{ queueLock };
	const double now = glfwGetTime();
	while (!eventQueue.empty() && eventQueue.top().is_due_at(now))
	{
		processEvent(eventQueue.top().get());
		eventQueue.pop();
	}
}

void EventSystem::processEvent(std::shared_ptr<Event> _event)
{
    //The loop looks a bit complicated, but a normal for-each loop cannot be used in the case of the deletion of a listener during the loop
    std::vector<ListenerFunction*> finishedListeners;
    std::vector<ListenerFunction*>::iterator found;
    while ((found = std::find_if(listeners.begin(), listeners.end(), [&](ListenerFunction* listener) {
        return std::find(finishedListeners.begin(), finishedListeners.end(), listener) == finishedListeners.end();
    })) != listeners.end())
    {
        ListenerFunction* listener = *found;
        (*listener)(_event.get());
        finishedListeners.push_back(listener);
    }
}

