#pragma once

#include <glm/vec2.hpp>
#include <string>

class Networker
{
public:
    Networker() {}

	virtual ~Networker() = default;

	virtual void sendCommand(uint32_t unitID, std::string action, glm::vec2 direction) = 0;
    virtual void sendChatMessage(struct ChatEntry& chatEntry) = 0;

	virtual void update() = 0;
};
