#include <iostream>
#include <string>
#include <stdexcept>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <renderer/OpenGLRenderer.h>
#include <renderer/ImGuiRenderer.h>
#include <imgui/imgui.h>
#include <imgui/opengl3_example/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_demo.cpp>
#include <renderer/Sprite.h>
#include <renderer/Texture.h>
#include <renderer/Textures.h>
#include <renderer/Sprites.h>
#include <gamestate/Gamestate.h>
#include <gamestate/Networker.h>
#include <input/Input.h>

#include "Game.h"

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

Game::Game() : localHost {"127.0.0.1"} {
    createWindow();
    initializeOpenGL();

    renderer = new OpenGLRenderer(window);
    imguiRenderer = new ImGuiRenderer(window);
    input = new Input(window);
    gameState = new Gamestate(localHost);

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
    delete gameState;
}


void Game::processInputs()
{
    /* Poll for and process events */
    glfwPollEvents();
}

void Game::update(double time)
{
    gameState->update(time);
}

void Game::render(double timeElapsed)
{
    imguiRenderer->preRender();
    renderer->preDraw();

    gameState->draw(*renderer);

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
        input->updateKeyStatus();
        update(elapsed);
        render(elapsed);
        lastTime = current;
    }
}
