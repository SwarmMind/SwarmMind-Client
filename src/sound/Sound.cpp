#include <sound/Sound.h>
#include <SFML/Audio.hpp>

using namespace std;

Sound::Sound()
{
}

Sound::~Sound() {

}

void Sound::play() {
    
    sf::SoundBuffer buffer;
    buffer.loadFromFile("sound/shot_and_reload.wav");
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setVolume(10);
    sound.play();
    
    while (sound.getStatus() == sf::Sound::Playing)
        ;

    /*
    sound/next_round.wav
    sound/shot_and_reload.wav
    sound/unit_walking.wav
    */

}

