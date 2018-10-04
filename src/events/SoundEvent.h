#pragma once

#include <memory>

#include <events/Event.h>

enum class SoundEnum;

class SoundEvent : public Event
{
public:
	SoundEvent(SoundEnum sound)
		: m_sound {sound}
	{}

	SoundEnum m_sound;
};