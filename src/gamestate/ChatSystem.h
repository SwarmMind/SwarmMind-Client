#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <vector>
#include <input/Input.h>
#include <deque>

class Networker;
class Renderer;

struct ChatEntry
{
    double m_timeStamp;
    glm::vec2 m_position;
    std::string m_user;
    std::string m_text;
};

class ChatSystem
{
public:
	ChatSystem(Input& input, Networker& networker);
	virtual ~ChatSystem() = default;
	
    void update(double deltaTime, double timeStamp);

    void openPopup();
    void drawPopup(double timeStamp);

    void draw(Renderer& renderer);

    float calculateAlpha(Camera &camera);

    void drawChats(Renderer& renderer);
    void drawNameInput(Renderer& renderer);

    void buildChatEntry(double timeStamp);
protected:
    void addChat(ChatEntry chat);

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