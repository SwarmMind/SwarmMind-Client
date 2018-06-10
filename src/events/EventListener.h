#pragma once

#include <functional>
#include <events/EventSystem.h>
class Event;

template <class T>
class EventListener
{
public:
	EventListener(EventSystem& system)
		: _eventSystem { system }
	{
		receiveEventFunction = [this](Event* _event){
			T* concreteEvent = dynamic_cast<T*>(_event);
			if (concreteEvent != nullptr)
			{
				this->receiveEvent(concreteEvent);
			}
		};
		_eventSystem.registerListener(&receiveEventFunction);
	}

	virtual ~EventListener() 
	{
		_eventSystem.removeListener(&receiveEventFunction);
	}

	virtual void receiveEvent(T* event) = 0;
private:
	EventSystem& _eventSystem;
	std::function<void(Event*)> receiveEventFunction;
};
