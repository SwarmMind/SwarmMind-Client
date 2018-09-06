#include <gamestate/TutorialNetworker.h>
#include <events/EventSystem.h>
#include <events/InitStateEvent.h>

TutorialNetworker::TutorialNetworker(class EventSystem& eventSystem)
    : m_eventSystem{eventSystem}
{}

void TutorialNetworker::sendCommand(uint32_t unitID, std::string action, glm::vec2 direction)
{

}

void TutorialNetworker::sendChatMessage(struct ChatEntry& chatEntry)
{

}

void TutorialNetworker::update()
{

}

void TutorialNetworker::begin()
{
    InitStateEvent startEvent;
    Configuration config;
    config.m_roundTime = m_roundTime;
    config.m_sizeX = m_mapSize;
    config.m_sizeY = m_mapSize;
    startEvent.m_config = config;

    glm::vec2 unitPosition(m_mapSize / 2, m_mapSize / 2);
    std::map<uint32_t, Unit> units;
    units.emplace(m_unitID, Unit(m_unitID, unitPosition));
    startEvent.m_state = new Gamestate(m_eventSystem, units, {});

    m_eventSystem.processEvent(&startEvent);
}

