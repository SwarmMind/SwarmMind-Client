#include <sound/Sounds.h>

Sounds::Sounds(EventSystem& event_system)
	: m_event_system{event_system}
	, EventListener<CommandEvent>(event_system)
{
    for (auto pair : soundFiles)
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
    std::remove_if(m_sounds.begin(), m_sounds.end(), [](sf::Sound& sound) {return sound.getStatus() == sf::Sound::Stopped;});
}

void Sounds::receiveEvent(CommandEvent * event)
{
	switch (event->command->type()) {
	case CommandType::Move:
		play(SoundEnum::Walk);
		break;
	case CommandType::Attack:
		play(SoundEnum::Attack1);
		break;
	case CommandType::Damage:
		play(SoundEnum::Hit1);
		break;
	case CommandType::Die:
		play(SoundEnum::Die1);
		break;
	default:
		break;
	}
}

void Sounds::inMainMenu(bool in_menu) {
	m_main_menu = in_menu;

	background.stop();

	if (in_menu) {
		background = sf::Sound{ m_buffers.at(SoundEnum::Menu) };
	} else {
		background = sf::Sound{ m_buffers.at(SoundEnum::Background1) };
	}
	background.play();
}

const map<SoundEnum, std::string> Sounds::soundFiles {
		{ SoundEnum::Background1, "sound/background_songs/background_1.wav" }, // these songs aren't implemented yet
		/*
		{ SoundEnum::Background2, "sound/background_songs/background_2.wav" }, //
		{ SoundEnum::Background3, "sound/background_songs/background_3.wav" }, //
		{ SoundEnum::Background4, "sound/background_songs/background_4.wav" }, //
		*/
		{ SoundEnum::Menu, "sound/menu_songs/menu_and_tutorial.wav" }, //
		/*
		{ SoundEnum::WalkCommand, "sound/given_command/walk_command_given.wav" }, //
		*/
		{ SoundEnum::Walk, "sound/unit_walk/unit_walking.wav" },
		/*
		{ SoundEnum::AttackCommand, "sound/given_command/shoot_command_given.wav" }, //
		*/
		{ SoundEnum::Attack1, "sound/unit_attack/shoot_1.wav" },
		/*
		{ SoundEnum::Attack2, "sound/unit_attack/shoot_2.wav" },
		{ SoundEnum::Attack3, "sound/unit_attack/shoot_3.wav" },
		*/
		{ SoundEnum::Hit1, "sound/monster_hit/got_hitten_scream_1.wav" },
		/*
		{ SoundEnum::Hit2, "sound/monster_hit/got_hitten_scream_2.wav" },
		{ SoundEnum::Hit3, "sound/monster_hit/got_hitten_scream_3.wav" },
		{ SoundEnum::Hit4, "sound/monster_hit/got_hitten_scream_4.wav" },
		{ SoundEnum::Hit5, "sound/monster_hit/got_hitten_scream_5.wav" },
		*/
		{ SoundEnum::Die1, "sound/monster_die/dying_scream_1.wav" },
		/*
		{ SoundEnum::Die2, "sound/monster_die/dying_scream_2.wav" },
		{ SoundEnum::Die3, "sound/monster_die/dying_scream_3.wav" },
		{ SoundEnum::Die4, "sound/monster_die/dying_scream_4.wav" },
		{ SoundEnum::Die5, "sound/monster_die/dying_scream_5.wav" },

		{ SoundEnum::NextRound1, "sound/next_round/next_round.wav" }, //
		{ SoundEnum::NextRound2, "sound/next_round/next_round_2.wav" }, //
		{ SoundEnum::EndingRound, "sound/next_round/ticking.wav" } //

		*/
};

