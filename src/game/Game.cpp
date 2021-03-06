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
    glbinding::Binding::initialize(glfwGetProcAddress, false);
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
	glfwSwapInterval(1);

    initializeOpenGL();

    return window;
}

Game::Game()
	: m_sounds{ m_eventSystem }
    , m_window { createWindow() }
    , m_camera { m_window, 10, 10, 11 }
    , m_renderer { m_window, m_camera }
    , m_imguiRenderer{ m_window }
    , m_input { m_window, &m_camera }
    , m_sprites { m_textures }
{
	initializeImGui();

	openMainMenu();
}

void Game::connectTo(std::string address, unsigned int port)
{
	m_settings.m_hostname = address;
	m_settings.m_port = port;
	m_settings.save();

    m_renderer.clearStaticData();
	m_menu = std::make_unique<ConnectedState>(*this, m_renderer, m_input, m_eventSystem, m_settings);

	m_sounds.inMainMenu(false);
}

void Game::openMainMenu()
{
	m_settings.read();

    m_menu = nullptr; //Delete the ConnectedState first!
                    //Important, because otherwise it is still registered as an EventListener
	m_menu = std::make_unique<MainMenuState>(this, m_eventSystem, m_input, m_renderer, m_settings);
	m_sounds.inMainMenu(true);
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
    style.Colors[ImGuiCol_Text] = ImVec4(.2f, .2f, .2f, 1.0f);
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
	m_input.update(deltaTime);
}

void Game::update(double time, double timeStamp)
{
    m_imguiRenderer.preRender(); // required before any update in order for popups to work!
	m_eventSystem.update(time, timeStamp);
    m_menu->update(time, timeStamp);
	m_sounds.update();
}

void Game::render(double timeElapsed)
{
    m_renderer.preDraw();
	m_menu->draw(m_renderer);
	drawDebug(timeElapsed);
    m_renderer.draw(timeElapsed);
    m_imguiRenderer.render();
    glfwSwapBuffers(m_window);
}

void Game::drawDebug(double timeElapsed)
{
	static list<double> frameTimes;

	frameTimes.push_back(timeElapsed);
	while (frameTimes.size() > 60)
		frameTimes.pop_front();

	if (m_input.isActionPressed(Debug))
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
			ImGui::TextColored(ImVec4(0.7f, 0.1f, 0.1f, 0.5f), "Leon");
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 0.5f), "My Fancy chat window!");
		}
		ImGui::End();

		ImGui::Text("width: %f", width);
		ImGui::Text("height: %f", ImGui::GetTextLineHeightWithSpacing() * 2);

	}

	if (m_input.isActionJustPressed(Debug))
	{
		ParticleSystem::spawnShootParticles(glm::vec2(5, 5), glm::vec2(0.5, 0.5));
	}
}

void Game::loop() {
    /* Loop until the user closes the window */
    double lastTime = glfwGetTime(); 
    while (!glfwWindowShouldClose(m_window))
    {
        const double current = glfwGetTime();
        const double elapsed = current - lastTime;
        processInputs(elapsed);

        update(elapsed, current);
        render(elapsed);
        lastTime = current;
    }
}

