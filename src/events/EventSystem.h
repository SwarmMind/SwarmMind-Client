#pragma once

#include <map>
#include <typeinfo>
#include <typeindex>
#include <vector>
#include <functional>
#include <memory>
#include <queue>
#include <deque>
#include <mutex>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <events/Event.h>


struct TimedEvent {
	double m_due;
	std::shared_ptr<Event> m_event;

	TimedEvent(std::shared_ptr<Event> event, double due) 
		: m_event{event}
		, m_due{ due }
	{}
	
	bool is_due() const {
		return glfwGetTime() >= m_due;
	}

	bool operator<(const TimedEvent& rhs) const {
		return m_due < rhs.m_due;
	}
	bool operator>(const TimedEvent& rhs) const {
		return m_due > rhs.m_due;
	}
};

class EventSystem
{
public:
	using ListenerFunction = std::function<void(Event*)>;
	EventSystem();

	~EventSystem();

	void update(double deltaTime, double timeStamp);

	void registerListener(ListenerFunction* listener);
	void removeListener(ListenerFunction* listener);
	void postEvent(std::shared_ptr<Event> _event, double offset = 0.0);
	void processEvent(std::shared_ptr<Event> _event);
protected:
	std::vector<ListenerFunction*> listeners;

	std::mutex queueLock;
	std::priority_queue<TimedEvent, std::deque<TimedEvent>, std::greater<TimedEvent>> eventQueue;
};
