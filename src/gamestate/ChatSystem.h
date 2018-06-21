#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <vector>
#include <input/Input.h>


class Renderer;

struct ChatEntry
{
    glm::vec2 m_position;
    std::string m_user;
    std::string m_text;
};

class ChatSystem
{
public:
	ChatSystem(Input& input);
	virtual ~ChatSystem() = default;
	
    void update(double deltaTime);

    void openPopup();

    void drawPopup();

    void buildChatEntry();

    void draw(Renderer& renderer);
protected:
    void addChat(ChatEntry chat);

    char m_chatInput[50] = "";

    std::vector<ChatEntry> m_chats;
    Input& m_input;

    glm::vec2 m_clickPosition;
    double m_doubleClickTimeout = -1;
    double m_doubleClickThreshold = 1;
private:
};