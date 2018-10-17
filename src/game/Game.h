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
	EventSystem m_eventSystem;

	Sounds m_sounds;
	
	GLFWwindow *m_window;
 	Camera m_camera;   
    OpenGLRenderer m_renderer;
    ImGuiRenderer m_imguiRenderer;
    Input m_input;

    std::unique_ptr<MenuState> m_menu;

	Textures m_textures;
	Sprites m_sprites;

	Settings m_settings;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;

	void connectTo(std::string address, unsigned int port = 3000);
	void openMainMenu();
	
	void loop();
};
