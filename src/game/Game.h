#pragma once

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <gamestate/Gamestate.h>
#include <gamestate/Networker.h>
#include <renderer/OpenGLRenderer.h>
#include <renderer/ImGuiRenderer.h>
#include <input/Input.h>

class Game {
    void createWindow();
    void initializeOpenGL();

    void processInputs();
    void update(double time);
    void render(double timeElapsed);

protected:
    Networker localHost;

    GLFWwindow *window;
    OpenGLRenderer *renderer;
    ImGuiRenderer *imguiRenderer;
    Input *input;
    Gamestate *gameState;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;

    void loop();
};
