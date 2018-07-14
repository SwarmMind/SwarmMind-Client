#include <sound/Sound.h>

Sound::Sound(sf::SoundBuffer buffer, deque<sf::Sound> soundInstances)
    : m_buffer { buffer }
    , m_soundInstances { soundInstances }
{

}

Sound::~Sound() {

}

void Sound::load(string wav) {
    m_buffer.loadFromFile("sound/" + wav);
}

void Sound::play() {
    m_soundInstances.push_back(sf::Sound(m_buffer));
    m_soundInstances.back().play();
}

void Sound::update() {
    for (int i = 0; i < m_soundInstances.size(); i++) {
        if (m_soundInstances[i].getStatus() == sf::Sound::Stopped) {
            m_soundInstances.erase(m_soundInstances.begin() + i);
            i--;
        }
    }
}

void Sound::count()
{
    cout << "size of soundInstances: " << m_soundInstances.size() << endl;
}

/*   
    sf::SoundBuffer buffer;
    buffer.loadFromFile("sound/" + wav);
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
        
    while (sound.getStatus() == sf::Sound::Playing)
        ;
*/

