#pragma once

#include <glbinding/gl41core/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
    
    GLFWwindow *window;
    OpenGLRenderer *renderer;
    ImGuiRenderer *imguiRenderer;
    Input *input;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;

	void connectTo(std::string address, unsigned int port = 3000);

    void loop();
};
