#pragma once

#include <glm/vec2.hpp>
#include <string>

struct ChatEntry
{
    double m_timeStamp;
    glm::vec2 m_position;
    std::string m_user;
    std::string m_text;
};