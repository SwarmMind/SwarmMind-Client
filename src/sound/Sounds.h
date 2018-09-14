#pragma once

#include <map>
#include <string>
#include <SFML/Audio.hpp>
#include <deque>
#include <iostream>
#include <list>

using namespace std;

enum class SoundEnum {
    Shoot,
    Walk,
    NextRound    
};

class Sounds
{
public:
    Sounds();
    ~Sounds();

    void play(SoundEnum soundName);
    void update();

private:
    list<sf::Sound> m_sounds;
    map<SoundEnum, sf::SoundBuffer> m_buffers;
    map<SoundEnum, std::string> soundFiles();
};