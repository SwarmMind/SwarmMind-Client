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
	m_listeners.emplace_back(listener);
}

void EventSystem::removeListener(ListenerFunction* listener)
{
	auto iterator = std::find(m_listeners.begin(), m_listeners.end(), listener);
	if (iterator != m_listeners.end())
	{
		m_listeners.erase(iterator);
	}
}

void EventSystem::postEvent(const std::shared_ptr<Event> event) {
	postEvent(TimedEvent{ event });
}

void EventSystem::postEvent(const TimedEvent& event)
{
	std::lock_guard<std::mutex> queueGuard{ m_queueLock };
	m_eventQueue.emplace(event);
}

void EventSystem::postEvents(const std::vector<TimedEvent>& events)
{
	std::lock_guard<std::mutex> queueGuard{ m_queueLock };
	std::for_each(events.cbegin(), events.cend(), [&](const auto& event) { m_eventQueue.push(event); });
}

void EventSystem::update(double deltaTime, double timeStamp)
{
	std::lock_guard<std::mutex> queueGuard{ m_queueLock };
	while (!m_eventQueue.empty() && m_eventQueue.top().is_due_at(timeStamp)) {
		processEvent(m_eventQueue.top().get());
		m_eventQueue.pop();
	}
}

void EventSystem::processEvent(std::shared_ptr<Event> event)
{
    //The loop looks a bit complicated, but a normal for-each loop cannot be used in the case of the deletion of a listener during the loop
    std::vector<ListenerFunction*> finishedListeners;
    std::vector<ListenerFunction*>::iterator found;
    while ((found = std::find_if(m_listeners.begin(), m_listeners.end(), [&](ListenerFunction* listener) {
        return std::find(finishedListeners.begin(), finishedListeners.end(), listener) == finishedListeners.end();
    })) != m_listeners.end())
    {
        ListenerFunction* listener = *found;
        (*listener)(event.get());
        finishedListeners.push_back(listener);
    }
}

