#pragma once

class EventSystem;

template <class T>
class EventListener
{
public:
	EventListener(EventSystem& system)
		: _eventSystem { system }
	{
		_eventSystem.registerListener<T>(this);
	}

	virtual ~EventListener() 
	{
		_eventSystem.removeListener<T>(this);
	}

	virtual void receiveEvent(T* event) = 0;
private:
	EventSystem& _eventSystem;
};
