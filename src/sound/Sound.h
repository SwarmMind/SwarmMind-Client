#pragma once

#include <input/Input.h> 

class Sound
{
public:
    Sound(Input* input);
    ~Sound();

    void play();

private:
    Input* m_input;
};