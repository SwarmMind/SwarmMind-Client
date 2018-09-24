#pragma once

#include <map>
#include <string>
#include <deque>
#include <iostream>
#include <list>

#include <SFML/Audio.hpp>

#include <events/EventListener.h>
#include <events/CommandEvent.h>

using namespace std;

enum class SoundEnum {
    Background1, 
    Background2, 
    Background3, 
    Background4,
    Menu,
    WalkCommand, 
    Walk,
    AttackCommand, 
    Attack1, 
    Attack2, 
    Attack3,
    Hit1, 
    Hit2, 
    Hit3, 
    Hit4, 
    Hit5,
    Die1, 
    Die2, 
    Die3, 
    Die4, 
    Die5,
    NextRound1, 
    NextRound2, 
    EndingRound 
};

class Sounds : public EventListener<CommandEvent>
{
public:
    Sounds() = delete;
	Sounds(const Sounds& rhs) = delete;
	Sounds(EventSystem& event_system);
    ~Sounds();

    void play(SoundEnum soundName);
    void update();

	void receiveEvent(CommandEvent* event);

private:
	EventSystem& m_event_system;
    list<sf::Sound> m_sounds;
    map<SoundEnum, sf::SoundBuffer> m_buffers;
    map<SoundEnum, std::string> soundFiles();
};