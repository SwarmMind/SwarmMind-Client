#include <iostream>
#include <string>
#include <stdexcept>

#include <glbinding/gl/gl.h>
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
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

	localHost.setConnectCallback(std::bind(&Game::onConnect, this));
	localHost.setDisconnectCallback(std::bind(&Game::onDisconnect, this));
	localHost.connect("127.0.0.1");

    renderer = new OpenGLRenderer(window);
    imguiRenderer = new ImGuiRenderer(window);
    input = new Input(window);

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 20);
    io.FontGlobalScale = 1.0f;
    ImGui::StyleColorsDark();

    renderer->setCamera(15, 15, 15);
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
               ImGui::Text(statusMessage.data());
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

void Game::onConnect()
{
}

void Game::onDisconnect()
{
	delete map;
	map = nullptr;
}
