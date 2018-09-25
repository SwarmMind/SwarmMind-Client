#pragma once

#include <cstdint>

#include <map>
#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include <gamestate/Entity.h>

#include <events/EventListener.h>
#include <events/CommandEvent.h>
#include <events/AccumulatedCommandsEvent.h>

class Configuration 
{
public:
    std::vector<glm::vec2> m_blockadePositions;

	uint32_t m_sizeX;
	uint32_t m_sizeY;

    double m_roundTime = 1.0;
};

class Gamestate : public EventListener<CommandEvent>, public EventListener<AccumulatedCommandsEvent>
{
public:
    Gamestate(const Gamestate& state) = default;
	Gamestate(class EventSystem& eventSystem);
	Gamestate(class EventSystem& eventSystem, std::map<uint32_t, Unit>& _units, const std::map<uint32_t, Monster>& _monsters);
	~Gamestate();

	void update(double deltaTime);
	void draw(class Renderer& renderer);

	Entity* getEntityByID(uint32_t ID);
	void deleteEntity(uint32_t ID);

	virtual void receiveEvent(CommandEvent* event) override;
	virtual void receiveEvent(AccumulatedCommandsEvent* event) override;

	std::map<uint32_t, Unit> units;
	std::map<uint32_t, Monster> monsters;
    class Map* m_map;
    void setMap(class Map* map);
};
