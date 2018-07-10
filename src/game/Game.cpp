#include <game/Game.h>
#include <list>
#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>

#include <glbinding/gl41core/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <renderer/OpenGLRenderer.h>
#include <renderer/ParticleSystem.h>
#include <renderer/ImGuiRenderer.h>
#include <renderer/Sprites.h>
#include <renderer/Textures.h>
#include <renderer/CommandVisualizer.h>
#include <input/Input.h>
#include <functional>
#include <menu/MainMenuState.h>
#include <menu/ConnectedState.h>

using namespace std;

GLFWwindow * Game::createWindow() {
	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(720, 720, "SwarmMind", nullptr, nullptr);
	if (!window)
	{
		throw std::runtime_error{
			"GLFW window could not be created!\n"
			"Please check your graphics driver. OpenGL v4.1 is required!\n"
		};
	}
    glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

    return window;
}

Game::Game()
    : window{ createWindow() }
    , camera{ window, 10, 10, 11 }
    , renderer{ window, camera }
    , imguiRenderer{ window }
    , input{ window, &camera }
    , sprites{ textures }
{
	initializeImGui();

	openMainMenu();
}

void Game::connectTo(std::string address, unsigned int port)
{
	menu = std::make_unique<ConnectedState>(*this, renderer, input, eventSystem, address, port);
}

void Game::openMainMenu()
{
	menu = std::make_unique<MainMenuState>(this);
}

void Game::initializeImGui()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 30);
	io.FontGlobalScale = 1.0f;
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0;
	
}

Game::~Game() {
}


void Game::processInputs(double deltaTime)
{
    /* Poll for and process events */
    glfwPollEvents();
	input.update(deltaTime);
}

void Game::update(double time, double timeStamp)
{
    imguiRenderer.preRender(); //Required before any update in order for popups to work!
    menu->update(time, timeStamp);
}

void Game::render(double timeElapsed)
{
    renderer.preDraw();

	menu->draw(renderer);
	drawDebug(timeElapsed);

    renderer.draw(timeElapsed);
    imguiRenderer.render();
    glfwSwapBuffers(window);
}

void Game::drawDebug(double timeElapsed)
{
	static list<double> frameTimes;

	frameTimes.push_back(timeElapsed);
	while (frameTimes.size() > 60)
		frameTimes.pop_front();

	if (input.isActionPressed(Debug))
	{
		double timeSum = 0;
		for (double frameTime : frameTimes)
			timeSum += frameTime;

		ImGui::Text("Current FPS: %f", 60 / timeSum);

		float width, height;

		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.5);
		if(ImGui::Begin("Test", nullptr,
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoInputs));
		{
			width = ImGui::GetWindowWidth();
			height = ImGui::GetWindowHeight();
			ImGui::TextColored(ImVec4(0.7, 0.1, 0.1, 0.5), "Leon");
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 0.5), "My Fancy chat window!");
		}
		ImGui::End();

		ImGui::Text("width: %f", width);
		ImGui::Text("height: %f", ImGui::GetTextLineHeightWithSpacing() * 2);

	}

	if (input.isActionJustPressed(Debug))
	{
		ParticleSystem::spawnShootParticles(glm::vec2(5, 5), glm::vec2(0.5, 0.5));
	}
}

void Game::loop() {
    /* Loop until the user closes the window */
    double lastTime = glfwGetTime(); 
    while (!glfwWindowShouldClose(window))
    {
        const double current = glfwGetTime();
        const double elapsed = current - lastTime;
        processInputs(elapsed);

        update(elapsed, current);
        render(elapsed);
        lastTime = current;
    }
}

