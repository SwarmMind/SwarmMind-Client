#pragma once

#include <renderer/OpenGLRenderer.h>
#include <renderer/ImGuiRenderer.h>
#include <input/Input.h>
#include <memory>
#include <menu/MenuState.h>
#include <game/Camera.h>
#include <events/EventSystem.h>
#include <sound/Sound.h>

//static bool g_isZoomingEnabled;

class Game {
    static GLFWwindow* createWindow();
    static void initializeOpenGL();

    void processInputs(double deltaTime);
    void update(double time, double timeStamp);
    void render(double timeElapsed);
	void drawDebug(double timeElapsed);
    void playAudio();

	void initializeImGui();
	
protected:   
	
	GLFWwindow *window;
 	Camera camera;   
    OpenGLRenderer renderer;
    ImGuiRenderer imguiRenderer;
    Input input;
    Sound sound;

	EventSystem eventSystem;

    std::unique_ptr<MenuState> menu;

	Textures textures;
	Sprites sprites;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;

	void connectTo(std::string address, unsigned int port = 3000);
	void openMainMenu();
	
	void loop();
};
