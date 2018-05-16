#include <iostream>
#include <string>
#include <stdexcept>

#include <glbinding/gl41core/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <renderer/OpenGLRenderer.h>
#include <renderer/ImGuiRenderer.h>
#include <gamestate/Gamestate.h>
#include <gamestate/Networker.h>
#include <input/Input.h>
#include <game/Game.h>
#include <functional>


void Game::createWindow() {
    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(720, 720, "SwarmMind", NULL, NULL);
    if (!window)
    {
        throw std::runtime_error {
            "GLFW window could not be created!" \
            "Please check your graphics driver. OpenGL v4.0 is required!\n"
        };
    }
}

void Game::initializeOpenGL() {
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    glbinding::Binding::initialize();

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, { "glGetError" });
    glbinding::setAfterCallback([](const glbinding::FunctionCall &)
    {
        const auto error = glGetError();
        if (error != 0)
            std::cout << "error: " << std::hex << error << std::endl;
    });
}

Game::Game() : map{nullptr} {
    createWindow();
    initializeOpenGL();

	enableCallbacks();

	localHost.connect("127.0.0.1");

    renderer = new OpenGLRenderer(window);
    imguiRenderer = new ImGuiRenderer(window);
    input = new Input(window);

	initializeImGui();

}


void Game::initializeImGui()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 20);
	io.FontGlobalScale = 1.0f;
	ImGui::StyleColorsDark();
}

using namespace std::placeholders;

void Game::enableCallbacks()
{
	localHost.setConnectCallback(std::bind(&Game::onConnect, this));
	localHost.setDisconnectCallback(std::bind(&Game::onDisconnect, this));
	localHost.setInitStateCallback(std::bind(&Game::onInitState, this, std::placeholders::_1, std::placeholders::_2));
	localHost.setStateCallback(std::bind(&Game::onState, this, std::placeholders::_1));
	localHost.setGameOverCallback(std::bind(&Game::onDisconnect, this));
}

Game::~Game() {
    delete input;
    delete renderer;
    delete imguiRenderer;
	delete map;
}


void Game::processInputs()
{
    /* Poll for and process events */
    glfwPollEvents();
	input->update();
}

void Game::update(double time)
{
	localHost.update();
	if (map != nullptr)
	{
		map->update(time);
	}
}

std::string Game::statusString() const {
    if (!localHost.isConnected()) {
        return "Connecting...";
    } else {
        return "Connected!";
    }
}

void Game::drawStatus() {
    const std::string statusMessage { statusString() };

	ImGui::SetNextWindowPos(ImVec2(30, 30), 0);
	ImGui::Begin(statusMessage.data(), nullptr
		, ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoInputs);
	{
		ImGui::Text("%s", statusMessage.data());
	}
	ImGui::End();
}

void Game::render(double timeElapsed)
{
    imguiRenderer->preRender();
    renderer->preDraw();

    drawStatus();

	if (map != nullptr)
	{
		map->draw(*renderer);
	}

    renderer->draw();
    imguiRenderer->render();
    glfwSwapBuffers(window);
}

void Game::loop() {
    /* Loop until the user closes the window */
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        const double current = glfwGetTime();
        const double elapsed = current - lastTime;
        processInputs();

        update(elapsed);
        render(elapsed);
        lastTime = current;
    }
}

void Game::onInitState(Configuration config, Gamestate *gamestate) {
	renderer->setCamera(config.sizeX / 2, config.sizeY / 2, config.sizeY / 2);
	
	map = new Map { *input, localHost, config };
    map->updateGameState(gamestate);
}

void Game::onState(Gamestate *gamestate) {
    map->updateGameState(gamestate);
}

void Game::onConnect()
{
}

void Game::onDisconnect()
{
	delete map;
	map = nullptr;
}
