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


class TimedEvent {
protected:
	double m_due;
	std::shared_ptr<Event> m_event;
	bool m_immediate;

public:
	TimedEvent(const std::shared_ptr<Event>& event)
		: m_event{ event }
		, m_due{ 0.0 }
		, m_immediate{ true }
	{}
	TimedEvent(const std::shared_ptr<Event>& event, const double offset) 
		: m_event{ event }
		, m_due{ glfwGetTime() + offset }
		, m_immediate{ false }
	{}
	TimedEvent() = delete;
	
	inline bool is_due_at(const double time) const noexcept {
		return m_immediate || time >= m_due;
	}
	bool is_due() const noexcept {
		return is_due_at(glfwGetTime());
	}
	std::shared_ptr<Event> get() const noexcept {
		return m_event;
	}

	bool operator<(const TimedEvent& rhs) const noexcept {
		return m_due < rhs.m_due;
	}
	bool operator>(const TimedEvent& rhs) const noexcept {
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

	void postEvent(const std::shared_ptr<Event> event);
	void postEvent(const TimedEvent& event);
	void postEvents(const std::vector<TimedEvent>& events);

	void processEvent(std::shared_ptr<Event> _event);
protected:
	std::vector<ListenerFunction*> listeners;

	std::mutex queueLock;
	std::priority_queue<TimedEvent, std::deque<TimedEvent>, std::greater<TimedEvent>> eventQueue;
};
