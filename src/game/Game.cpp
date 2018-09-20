#include <game/Game.h>
#include <list>
#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>

#include <glbinding/gl41core/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/types_to_string.h>
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

void Game::initializeOpenGL() {
    glbinding::Binding::initialize(glfwGetProcAddress);
    glbinding::Binding::setCallbackMaskExcept(glbinding::CallbackMask::After, { "glGetError" });
    glbinding::Binding::setAfterCallback([](const glbinding::FunctionCall &)
    {
        const auto error = glGetError();
        if (error != 0)
            std::cout << "error: " << std::hex << error << std::endl;
    });
    std::cout << "OpenGL Version: " << glbinding::aux::ContextInfo::version().toString() << std::endl;
    std::cout << "GPU Vendor: " << glbinding::aux::ContextInfo::vendor() << std::endl;
}

GLFWwindow * Game::createWindow() {
	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(1280, 720, "SwarmMind", nullptr, nullptr);
	if (!window)
	{
		throw std::runtime_error{
			"GLFW window could not be created!\n"
			"Please check your graphics driver. OpenGL v4.1 is required!\n"
		};
	}
    glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

    initializeOpenGL();

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
	settings.hostname = address;
	settings.port = port;
	settings.save();

    renderer.clearStaticData();
	menu = std::make_unique<ConnectedState>(*this, renderer, input, eventSystem, settings);
}

void Game::openMainMenu()
{
	settings.read();

    menu = nullptr; //Delete the ConnectedState first!
                    //Important, because otherwise it is still registered as an EventListener
	menu = std::make_unique<MainMenuState>(this, eventSystem, input, renderer, settings.hostname, settings.port);
}

void Game::initializeImGui()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 30);
	io.FontGlobalScale = 1.0f;
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0;
	
    //Colors
    ImVec4 windowBg = ImVec4(.9f, .9f, .9f, 1);
    ImVec4 buttonBg = ImVec4(.8f, .8f, .8f, 1);
    ImVec4 highlightColor = ImVec4(.9f, .7f, 0, 0.7f);
    ImVec4 activeColor = ImVec4(.9f, .7f, 0, 1.f);
    
    //Window style
    style.Colors[ImGuiCol_WindowBg] = windowBg;
    style.Colors[ImGuiCol_TitleBg] = windowBg;
    style.Colors[ImGuiCol_TitleBgActive] = buttonBg;
    style.Colors[ImGuiCol_PopupBg] = windowBg;

    //Window elements
    style.Colors[ImGuiCol_Text] = ImVec4(.2, .2, .2, 1);
    style.Colors[ImGuiCol_Button] = buttonBg;
    style.Colors[ImGuiCol_ButtonHovered] = highlightColor;
    style.Colors[ImGuiCol_ButtonActive] = activeColor;
    style.Colors[ImGuiCol_TextSelectedBg] = highlightColor;
    style.Colors[ImGuiCol_FrameBg] = buttonBg;
    style.Colors[ImGuiCol_Header] = buttonBg;
    style.Colors[ImGuiCol_HeaderHovered] = highlightColor;
    style.Colors[ImGuiCol_HeaderActive] = activeColor;
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
			| ImGuiWindowFlags_NoInputs))
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

