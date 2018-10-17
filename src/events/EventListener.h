#pragma once

#include <functional>
#include <events/EventSystem.h>
class Event;

template <class T>
class EventListener
{
public:
	EventListener() = delete;

	EventListener(EventSystem& system)
		: m_eventSystem { system }
	{
		m_receiveEventFunction = [this](Event* event){
			T* concreteEvent = dynamic_cast<T*>(event);
			if (concreteEvent != nullptr)
			{
				this->receiveEvent(concreteEvent);
			}
		};
		m_eventSystem.registerListener(&m_receiveEventFunction);
	}

	virtual ~EventListener() 
	{
		m_eventSystem.removeListener(&m_receiveEventFunction);
	}

	virtual void receiveEvent(T* event) = 0;
private:
	EventSystem& m_eventSystem;
	std::function<void(Event*)> m_receiveEventFunction;
};
