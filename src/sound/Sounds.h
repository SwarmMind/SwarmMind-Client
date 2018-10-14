#pragma once

#include <map>
#include <string>
#include <deque>
#include <iostream>
#include <list>
#include <random>
#include <memory>

#include <SFML/Audio.hpp>

#include <events/EventListener.h>
#include <events/CommandEvent.h>
#include <events/SoundEvent.h>

using namespace std;

enum class SoundEnum {
    Background,
    Menu,
    WalkCommand, 
    Walk,
    AttackCommand, 
    Attack,
    Hit,
    Die,
    NextRound,
    EndingRound 
};

class Sounds : public EventListener<CommandEvent>, public EventListener<SoundEvent>
{
public:
    Sounds() = delete;
	Sounds(const Sounds& rhs) = delete;
	Sounds(EventSystem& event_system);
    ~Sounds();

    void play(SoundEnum soundName);
	void playRandom(SoundEnum soundName);
    void update();

	void receiveEvent(CommandEvent* event) override;
	void receiveEvent(SoundEvent* event) override;

	void inMainMenu(bool in_menu);

protected:
	EventSystem& m_event_system;
    list<sf::Sound> m_sounds;
    multimap<SoundEnum, std::unique_ptr<sf::SoundBuffer>> m_buffers;

	std::vector<std::shared_ptr<sf::Music>> menu_music;
	std::vector<std::shared_ptr<sf::Music>> game_music;
	std::shared_ptr<sf::Music> current_music;

	void playBackground();

	sf::SoundBuffer& selectRandom(SoundEnum soundName);

private:
	bool m_main_menu = true;

	std::mt19937_64 random;

	static const multimap<SoundEnum, std::string> soundFiles;
};