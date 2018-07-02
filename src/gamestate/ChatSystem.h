#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <vector>
#include <input/Input.h>
#include <deque>
#include <gamestate/ChatEntry.h>
#include <events/ChatEvent.h>
#include <events/EventListener.h>
#include <events/EventSystem.h>

class Networker;
class Renderer;

class ChatSystem : public EventListener<ChatEvent>
{
public:
	ChatSystem(Input& input, Networker& networker, EventSystem& eventSystem);
	virtual ~ChatSystem() = default;
	
    void update(double deltaTime, double timeStamp);

    void openPopup();
    void drawPopup(double timeStamp);

    void draw(Renderer& renderer);

    float calculateAlpha(Camera &camera);

    void drawChats(Renderer& renderer);
    void drawNameInput(Renderer& renderer);

    void buildChatEntry(double timeStamp);

    virtual void receiveEvent(ChatEvent* event) override;

protected:
    void addChat(ChatEntry chat);

    double m_lastTimeStamp;

    unsigned m_chatInputBufferSize = 50;
    char m_chatInput[50] = "";
    unsigned m_userNameBufferSize = 16;
    char m_userName[16] = "Player";

    std::deque<ChatEntry> m_chats;
    Input& m_input;
    Networker& m_networker;

    glm::vec2 m_clickPosition;
    double m_doubleClickTimeout = -1;
    double m_doubleClickThreshold = 1;
    double m_chatVisibilityTimeout = 10;
private:
};