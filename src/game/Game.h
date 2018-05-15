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
#include <gamestate/Map.h>

class Game {
    void createWindow();
    void initializeOpenGL();

    void processInputs();
    void update(double time);
    void render(double timeElapsed);

    std::string statusString() const;
    void drawStatus();

    void onInitState(Configuration config, Gamestate *state);
    void onState(Gamestate *state);
	void onConnect();
	void onDisconnect();
	
	void initializeImGui();
	void enableCallbacks();

protected:
    Networker localHost;

    GLFWwindow *window;
    OpenGLRenderer *renderer;
    ImGuiRenderer *imguiRenderer;
    Input *input;
	Map* map;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;


    void loop();
};
