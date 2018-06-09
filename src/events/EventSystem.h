#pragma once

#include <events/EventListener.h>
#include <events/Event.h>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <vector>

class EventSystem
{
public:
	EventSystem();

	~EventSystem();

	template <class T>
	void registerListener(EventListener<T>* rawListener)
	{
		EventListener<Event>* listener = reinterpret_cast<EventListener<Event>*>(rawListener);
		listeners[typeid(T)].push_back(listener);
	}
	
	template <class T>
	void removeListener(EventListener<T>* rawListener)
	{
		EventListener<Event>* listener = reinterpret_cast<EventListener<Event>*>(rawListener);

		std::vector<EventListener<Event>*>& typeListeners = listeners[typeid(T)];
		auto iterator = std::find(typeListeners.begin(), typeListeners.end(), listener);
		if (iterator != typeListeners.end())
		{
			typeListeners.erase(iterator);
		}
	}

	template <class T>
	void processEvent(T* _event)
	{
		std::vector<EventListener<Event>*> typeListeners = listeners[typeid(T)];

		for (EventListener<Event>*& rawListener : typeListeners)
		{
			EventListener<T>* listener = reinterpret_cast<EventListener<T>*>(rawListener);
			listener->receiveEvent(_event);
		}
	}
private:
	std::map<std::type_index, std::vector<EventListener<Event>*>> listeners;
};
