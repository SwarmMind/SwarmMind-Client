#include <random>
#include <cstdint>
#include <sound/Sounds.h>

Sounds::Sounds(EventSystem& eventSystem)
	: m_eventSystem{eventSystem}
	, EventListener<CommandEvent>(eventSystem)
	, EventListener<SoundEvent>(eventSystem)
	, m_random{ std::random_device{}() }
	, m_currentMusic{ nullptr }
{
	const size_t filecount = m_soundFiles.size();
	uint32_t current_file = 1;
    for (const auto& pair: m_soundFiles) {
		bool success;

		std::cout << "[" << current_file++ << "/" << filecount << "] Loading \"" << pair.second << "\"... ";
		std::cout.flush();
		switch (pair.first) {
		case SoundEnum::Background:
			m_gameMusic.emplace_back(std::make_shared<sf::Music>());
			success = m_gameMusic.back()->openFromFile(pair.second);
			break;
		case SoundEnum::Menu:
			m_menuMusic.emplace_back(std::make_shared<sf::Music>());
			success = m_menuMusic.back()->openFromFile(pair.second);
			break;
		default:
			const auto it = m_buffers.emplace(pair.first, new sf::SoundBuffer{});
			success = it->second->loadFromFile(pair.second);
			break;
		};
		if (success) std::cout << "[done]" << std::endl;
    }
}

Sounds::~Sounds()
{
}


void Sounds::update() {
    m_sounds.remove_if([](sf::Sound& sound) {return sound.getStatus() == sf::Sound::Stopped;});
	if (m_currentMusic->getStatus() == sf::Sound::Stopped) {
		playBackground();
	}
}

void Sounds::receiveEvent(SoundEvent * event)
{
	playRandom(event->m_sound);
}

void Sounds::receiveEvent(CommandEvent * event)
{
	switch (event->m_command->type()) {
	case CommandType::Move:
		play(SoundEnum::Walk);
		break;
	case CommandType::Attack:
		playRandom(SoundEnum::Attack);
		break;
	case CommandType::Damage:
		playRandom(SoundEnum::Hit);
		break;
	case CommandType::Die:
		playRandom(SoundEnum::Die);
		break;
	default:
		break;
	}
}

void Sounds::play(SoundEnum soundName) {
	m_sounds.emplace_back(*m_buffers.find(soundName)->second);
	m_sounds.back().play();
}

sf::SoundBuffer& Sounds::selectRandom(SoundEnum soundName) {
	const auto range = m_buffers.equal_range(soundName);
	const auto sound_count = std::distance(range.first, range.second);
	if (sound_count == 1) {
		return *range.first->second;
	} else {
		std::uniform_int_distribution<std::multimap<SoundEnum, sf::SoundBuffer>::iterator::difference_type> dist{ 0, sound_count - 1 };
		auto advance = dist(m_random);
		auto it = range.first;
		while (advance--) it++;
		return *it->second;
	}
}

void Sounds::playRandom(SoundEnum soundName) {
	m_sounds.emplace_back(selectRandom(soundName));
	m_sounds.back().play();
}

void Sounds::playBackground() {
	using dist_type = std::uniform_int_distribution<std::vector<sf::Music>::size_type>;
	if (m_currentMusic) m_currentMusic->stop();

	if (m_mainMenu) {
		dist_type dist{ 0, m_menuMusic.size()-1 };
		m_currentMusic = m_menuMusic.at(dist(m_random));
	}
	else {
		dist_type dist{ 0, m_gameMusic.size()-1 };
		m_currentMusic = m_gameMusic.at(dist(m_random));
	}
	m_currentMusic->setVolume(70.0);

	m_currentMusic->play();
}

void Sounds::inMainMenu(bool in_menu) {
	m_mainMenu = in_menu;
	
	playBackground();
}

const multimap<SoundEnum, std::string> Sounds::m_soundFiles {
		{ SoundEnum::Background, "sound/background_songs/background_1.flac" },
		{ SoundEnum::Background, "sound/background_songs/background_2.flac" },
		{ SoundEnum::Background, "sound/background_songs/background_3.flac" },
		{ SoundEnum::Background, "sound/background_songs/background_4.flac" },

		{ SoundEnum::Menu, "sound/menu_songs/menu_and_tutorial.flac" },
		
		{ SoundEnum::WalkCommand, "sound/given_command/walk_command_given.flac" }, 
		{ SoundEnum::Walk, "sound/unit_walk/unit_walking.flac" },
		
		{ SoundEnum::AttackCommand, "sound/given_command/shoot_command_given.flac" },
		{ SoundEnum::Attack, "sound/unit_attack/shoot_1.flac" },
		{ SoundEnum::Attack, "sound/unit_attack/shoot_2.flac" },
		{ SoundEnum::Attack, "sound/unit_attack/shoot_3.flac" },
		
		{ SoundEnum::Hit, "sound/monster_hit/hit_scream_1.flac" },
		{ SoundEnum::Hit, "sound/monster_hit/hit_scream_2.flac" },
		{ SoundEnum::Hit, "sound/monster_hit/hit_scream_3.flac" },
		{ SoundEnum::Hit, "sound/monster_hit/hit_scream_4.flac" },
		{ SoundEnum::Hit, "sound/monster_hit/hit_scream_5.flac" },
		
		{ SoundEnum::Die, "sound/monster_die/dying_scream_1.flac" },
		{ SoundEnum::Die, "sound/monster_die/dying_scream_2.flac" },
		{ SoundEnum::Die, "sound/monster_die/dying_scream_3.flac" },
		{ SoundEnum::Die, "sound/monster_die/dying_scream_4.flac" },
		{ SoundEnum::Die, "sound/monster_die/dying_scream_5.flac" },
		
		{ SoundEnum::NextRound, "sound/next_round/next_round_1.flac" },
		{ SoundEnum::NextRound, "sound/next_round/next_round_2.flac" },
		{ SoundEnum::EndingRound, "sound/next_round/ticking.flac" } //
};

