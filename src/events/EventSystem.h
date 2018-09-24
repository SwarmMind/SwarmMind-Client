#pragma once

#include <events/Event.h>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <vector>
#include <functional>
#include <memory>
#include <queue>
#include <mutex>


class EventSystem
{
public:
	using ListenerFunction = std::function<void(Event*)>;
	EventSystem();

	~EventSystem();

	void update(double deltaTime, double timeStamp);

	void registerListener(ListenerFunction* listener);
	void removeListener(ListenerFunction* listener);
	void postEvent(std::shared_ptr<Event> _event);
	void processEvent(std::shared_ptr<Event> _event);
protected:
	std::vector<ListenerFunction*> listeners;

	std::mutex queueLock;
	std::queue<std::shared_ptr<Event>> eventQueue;
};
