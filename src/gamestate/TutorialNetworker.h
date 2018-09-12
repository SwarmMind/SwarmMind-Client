#pragma once

#include <gamestate/Networker.h>
#include <glm/vec2.hpp>


struct TutorialMessage {
    std::string m_message;
    glm::vec2 m_position;
    float m_delay;
};

class TutorialNetworker : public Networker
{
public:
	TutorialNetworker(class EventSystem& eventSystem);
	virtual ~TutorialNetworker() = default;

    virtual void sendCommand(uint32_t unitID, std::string action, glm::vec2 direction) override;
    virtual void sendChatMessage(struct ChatEntry& chatEntry) override;

    virtual void update(double deltaTime, double timeStamp) override;

    void begin();
protected:
	
    class EventSystem& m_eventSystem;
    float m_roundTime = 10;
    float m_mapSize = 20;
    uint32_t m_unitID = 1;
private:
};