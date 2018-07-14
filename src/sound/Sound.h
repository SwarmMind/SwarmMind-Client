#pragma once

#include <string>
#include <SFML/Audio.hpp>
#include <deque>
#include <iostream>

using namespace std;

class Sound
{
public:
    Sound(sf::SoundBuffer buffer, deque<sf::Sound> soundInstances);
    ~Sound();

    void load(string wav);
    void play();
    void update();
    void count(); // for debugging

private:
    sf::SoundBuffer m_buffer;
    deque<sf::Sound> m_soundInstances;
};