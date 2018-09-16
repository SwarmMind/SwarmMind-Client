#pragma once

#include <map>
#include <string>
#include <SFML/Audio.hpp>
#include <deque>
#include <iostream>
#include <list>

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