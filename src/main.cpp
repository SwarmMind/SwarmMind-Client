/*

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <glbinding/gl41core/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <game/Game.h>

int main(void)
{
	if (!glfwInit()) 
	{
		return EXIT_FAILURE;
	}

    try {
        Game game;
        game.loop();
    } catch (std::runtime_error e) {
        std::cerr << e.what() << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}*/



#include <events/EventSystem.h>
#include <events/EventListener.h>
#include <events/CommandEvent.h>
#include <iostream>
EventSystem _system;

class myEventListener : public EventListener<CommandEvent>, public EventListener<Event>
{
public:
	myEventListener(EventSystem& eventSystem) 
	: EventListener<CommandEvent>(eventSystem)
	, EventListener<Event>(eventSystem)
	{}

	virtual ~myEventListener() = default;

	virtual void receiveEvent(CommandEvent* event) override
	{
		std::cout << event->myString << std::endl;
	}

	virtual void receiveEvent(Event* event) override
	{
		std::cout << "received empty event!" << std::endl;
	}

};

class AnotherListener : public EventListener<CommandEvent>
{
public:
	AnotherListener(EventSystem& eventSystem)
		: EventListener<CommandEvent>(eventSystem)
	{}

	virtual ~AnotherListener() = default;

	virtual void receiveEvent(CommandEvent* event)
	{
		std::cout << "event String:" << event->myString << std::endl;
	}
};

int main(void)
{
	{
	myEventListener myListener(_system);

	CommandEvent _event;
	_event.myString = "hello world!";

	_system.processEvent(&_event);
		
	}
	AnotherListener anotherListener(_system);
	
	Event _noEvent;
	_system.processEvent(&_noEvent);

}