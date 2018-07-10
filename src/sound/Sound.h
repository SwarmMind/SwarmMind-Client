#pragma once

#include <input/Input.h> 

class Sound
{
public:
    Sound();
    ~Sound();

    void play();

private:
    Input* m_input;
};