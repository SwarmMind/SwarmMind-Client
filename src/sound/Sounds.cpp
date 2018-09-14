#include <sound/Sounds.h>

Sounds::Sounds()
{
    map<SoundEnum, std::string> bufferInitializers = soundFiles();
    for (auto pair : bufferInitializers)
    {
        m_buffers[pair.first] = sf::SoundBuffer();
        m_buffers[pair.first].loadFromFile(pair.second);
    }
}

Sounds::~Sounds()
{
}

void Sounds::play(SoundEnum soundName){
    m_sounds.emplace_back(m_buffers.at(soundName));
    m_sounds.back().play();
}

void Sounds::update() {
    std::remove_if(m_sounds.begin(), m_sounds.end(), [](sf::Sound& sound) {return sound.getStatus() == sf::Sound::Stopped; });
}

map<SoundEnum, std::string> Sounds::soundFiles() 
{
    return map<SoundEnum, std::string>({ 
        {SoundEnum::Shoot, "sound/shot_and_reload.wav"},
        {SoundEnum::Walk, "sound/unit_walking.wav"},
        {SoundEnum::NextRound, "sound/next_round.wav"}    
    });
}

