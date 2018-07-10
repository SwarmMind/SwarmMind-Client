#include <sound/Sound.h>
#include <SFML/Audio.hpp>

using namespace std;

Sound::Sound(Input* input)
    : m_input{ input }
{
}

Sound::~Sound() {

}

void Sound::play() {
    sf::SoundBuffer buffer;
    buffer.loadFromFile("gun.wav");
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
}

