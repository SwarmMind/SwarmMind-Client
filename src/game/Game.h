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
    void createWindow();
    void initializeOpenGL();

    void processInputs(double deltaTime);
    void update(double time, double timeStamp);
    void render(double timeElapsed);
	void drawDebug(double timeElapsed);
    void playAudio();

	void initializeImGui();
	
	std::unique_ptr<MenuState> menu;

protected:
    
    class GLFWwindow *window;
    OpenGLRenderer *renderer;
    ImGuiRenderer *imguiRenderer;
    Input *input;
	Camera *camera;
    Sound *sound;
	
	class Textures* textures;
	class Sprites* sprites;
	EventSystem eventSystem;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;

	void connectTo(std::string address, unsigned int port = 3000);
	void openMainMenu();
	
	void loop();
};
