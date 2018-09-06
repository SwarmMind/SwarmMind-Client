#pragma once

#include <gamestate/Networker.h>

class TutorialNetworker : public Networker
{
public:
	TutorialNetworker(class EventSystem& eventSystem);
	virtual ~TutorialNetworker() = default;

    virtual void sendCommand(uint32_t unitID, std::string action, glm::vec2 direction) override;
    virtual void sendChatMessage(struct ChatEntry& chatEntry) override;

    virtual void update() override;

    void begin();
protected:
	
    class EventSystem& m_eventSystem;
    float m_roundTime = 10;
    float m_mapSize = 20;
    uint32_t m_unitID = 1;
private:
};