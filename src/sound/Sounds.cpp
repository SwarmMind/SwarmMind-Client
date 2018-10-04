#include <random>
#include <cstdint>
#include <sound/Sounds.h>

Sounds::Sounds(EventSystem& event_system)
	: m_event_system{event_system}
	, EventListener<CommandEvent>(event_system)
	, random{ std::random_device{}() }
	, current_music{ nullptr }
{
	const size_t filecount = soundFiles.size();
	uint32_t current_file = 1;
    for (const auto pair: soundFiles) {
		bool success;

		std::cout << "[" << current_file++ << "/" << filecount << "] Loading \"" << pair.second << "\"... ";
		std::cout.flush();
		switch (pair.first) {
		case SoundEnum::Background:
			game_music.emplace_back(std::make_shared<sf::Music>());
			success = game_music.back()->openFromFile(pair.second);
			break;
		case SoundEnum::Menu:
			menu_music.emplace_back(std::make_shared<sf::Music>());
			success = menu_music.back()->openFromFile(pair.second);
			break;
		default:
			const auto it = m_buffers.emplace(pair.first, sf::SoundBuffer{});
			success = it->second.loadFromFile(pair.second);
			break;
		};
		if (success) std::cout << "[done]" << std::endl;
    }
	playBackground();
}

Sounds::~Sounds()
{
}


void Sounds::update() {
    std::remove_if(m_sounds.begin(), m_sounds.end(), [](sf::Sound& sound) {return sound.getStatus() == sf::Sound::Stopped;});
	if (current_music->getStatus() == sf::Sound::Stopped) {
		playBackground();
	}
}

void Sounds::receiveEvent(CommandEvent * event)
{
	switch (event->command->type()) {
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
	m_sounds.emplace_back(m_buffers.find(soundName)->second);
	m_sounds.back().play();
}

sf::SoundBuffer& Sounds::selectRandom(SoundEnum soundName) {
	const auto range = m_buffers.equal_range(soundName);
	std::uniform_int_distribution<std::multimap<SoundEnum, sf::SoundBuffer>::iterator::difference_type> dist{ 0, std::distance(range.first, range.second)-1 };
	auto advance = dist(random);
	auto it = range.first;
	while (advance--) it++;
	return it->second;
}

void Sounds::playRandom(SoundEnum soundName) {
	m_sounds.emplace_back(selectRandom(soundName));
	m_sounds.back().play();
}

void Sounds::playBackground() {
	using dist_type = std::uniform_int_distribution<std::vector<sf::Music>::size_type>;
	if (current_music) current_music->stop();

	if (m_main_menu) {
		dist_type dist{ 0, menu_music.size()-1 };
		current_music = menu_music.at(dist(random));
	}
	else {
		dist_type dist{ 0, game_music.size()-1 };
		current_music = game_music.at(dist(random));
	}
	current_music->setVolume(70.0);

	current_music->play();
}

void Sounds::inMainMenu(bool in_menu) {
	m_main_menu = in_menu;
	
	playBackground();
}

const multimap<SoundEnum, std::string> Sounds::soundFiles {
		{ SoundEnum::Background, "sound/background_songs/background_1.flac" },
		{ SoundEnum::Background, "sound/background_songs/background_2.flac" },
		{ SoundEnum::Background, "sound/background_songs/background_3.flac" },
		{ SoundEnum::Background, "sound/background_songs/background_4.flac" },

		{ SoundEnum::Menu, "sound/menu_songs/menu_and_tutorial.flac" },
		
		{ SoundEnum::WalkCommand, "sound/given_command/walk_command_given.wav" }, //
		{ SoundEnum::Walk, "sound/unit_walk/unit_walking.wav" },
		
		{ SoundEnum::AttackCommand, "sound/given_command/shoot_command_given.wav" }, //
		{ SoundEnum::Attack, "sound/unit_attack/shoot_1.wav" },
		{ SoundEnum::Attack, "sound/unit_attack/shoot_2.wav" },
		{ SoundEnum::Attack, "sound/unit_attack/shoot_3.wav" },
		
		{ SoundEnum::Hit, "sound/monster_hit/got_hitten_scream_1.wav" },
		{ SoundEnum::Hit, "sound/monster_hit/got_hitten_scream_2.wav" },
		{ SoundEnum::Hit, "sound/monster_hit/got_hitten_scream_3.wav" },
		{ SoundEnum::Hit, "sound/monster_hit/got_hitten_scream_4.wav" },
		{ SoundEnum::Hit, "sound/monster_hit/got_hitten_scream_5.wav" },
		
		{ SoundEnum::Die, "sound/monster_die/dying_scream_1.wav" },
		{ SoundEnum::Die, "sound/monster_die/dying_scream_2.wav" },
		{ SoundEnum::Die, "sound/monster_die/dying_scream_3.wav" },
		{ SoundEnum::Die, "sound/monster_die/dying_scream_4.wav" },
		{ SoundEnum::Die, "sound/monster_die/dying_scream_5.wav" },
		
		{ SoundEnum::NextRound, "sound/next_round/next_round.wav" }, //
		{ SoundEnum::NextRound, "sound/next_round/next_round_2.wav" }, //
		{ SoundEnum::EndingRound, "sound/next_round/ticking.wav" } //
};

