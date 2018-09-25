#pragma once

#include <renderer/OpenGLRenderer.h>
#include <renderer/ImGuiRenderer.h>
#include <input/Input.h>
#include <memory>
#include <menu/MenuState.h>
#include <game/Camera.h>
#include <events/EventSystem.h>
#include <game/Settings.h>
#include <sound/Sounds.h>

using namespace std;

class Game {
    static GLFWwindow* createWindow();
    static void initializeOpenGL();

    void processInputs(double deltaTime);
    void update(double time, double timeStamp);
    void render(double timeElapsed);
	void drawDebug(double timeElapsed);

	void initializeImGui();
	
protected:
	EventSystem eventSystem;

	Sounds sounds;
	
	GLFWwindow *window;
 	Camera camera;   
    OpenGLRenderer renderer;
    ImGuiRenderer imguiRenderer;
    Input input;

    std::unique_ptr<MenuState> menu;

	Textures textures;
	Sprites sprites;

	Settings settings;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;

	void connectTo(std::string address, unsigned int port = 3000);
	void openMainMenu();
	
	void loop();
};
