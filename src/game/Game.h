#pragma once

#include <renderer/OpenGLRenderer.h>
#include <renderer/ImGuiRenderer.h>
#include <input/Input.h>
#include <memory>
#include <menu/MenuState.h>

class Game {
    void createWindow();
    void initializeOpenGL();

    void processInputs();
    void update(double time);
    void render(double timeElapsed);
	void drawDebug(double timeElapsed);

	void initializeImGui();

	std::unique_ptr<MenuState> menu;

protected:
    
    class GLFWwindow *window;
    OpenGLRenderer *renderer;
    ImGuiRenderer *imguiRenderer;
    Input *input;

	class Textures* textures;
	class Sprites* sprites;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;

	void connectTo(std::string address, unsigned int port = 3000);
	void openMainMenu();

    void loop();
};
