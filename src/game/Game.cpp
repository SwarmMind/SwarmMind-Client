#include <game/Game.h>
#include <list>
#include <string>
#include <stdexcept>
#include <iostream>

#include <glbinding/gl41core/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <renderer/OpenGLRenderer.h>
#include <renderer/ParticleSystem.h>
#include <renderer/ImGuiRenderer.h>
#include <input/Input.h>
#include <functional>
#include <menu/MainMenuState.h>
#include <menu/ConnectedState.h>

#include <renderer/Sprites.h>
#include <renderer/Textures.h>


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

Game::Game()
{
    createWindow();
    initializeOpenGL();

    renderer = new OpenGLRenderer(window);
	renderer->setCamera(5, 5, 6);
    imguiRenderer = new ImGuiRenderer(window);
    input = new Input(window);

	textures = new Textures();
	sprites = new Sprites(*textures);

	initializeImGui();

	openMainMenu();
}


void Game::connectTo(std::string address, unsigned int port)
{
	menu = std::make_unique<ConnectedState>(*this, *sprites, *input, address, port);
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
}

Game::~Game() {
	delete sprites;
	delete textures;

    delete input;
    delete renderer;
    delete imguiRenderer;
}


void Game::processInputs()
{
    /* Poll for and process events */
    glfwPollEvents();
	input->update();
}

void Game::update(double time)
{
	menu->update(time);
}

void Game::render(double timeElapsed)
{
    imguiRenderer->preRender();
    renderer->preDraw();

	menu->draw(*renderer);
	drawDebug(timeElapsed);

    renderer->draw(timeElapsed);
    imguiRenderer->render();
    glfwSwapBuffers(window);
}

void Game::drawDebug(double timeElapsed)
{
	static list<double> frameTimes;

	frameTimes.push_back(timeElapsed);
	while (frameTimes.size() > 60)
		frameTimes.pop_front();

	if (input->isActionPressed(Debug))
	{
		double timeSum = 0;
		for (double frameTime : frameTimes)
			timeSum += frameTime;

		ImGui::Text("Current FPS: %f", 60 / timeSum);
	}

	if (input->isActionJustPressed(Debug))
	{
		ParticleSystem::spawnBloodParticles(5, 5);
/*

		ParticleSystem particles;

		for (size_t i = 0; i < 10000; i++)
		{
			particles.dynamicData.push_back(6.5f);
			particles.dynamicData.push_back(5.5f);
			particles.dynamicData.push_back((float)std::rand() / (float)RAND_MAX * 1.f);
			particles.dynamicData.push_back(0.f);

			particles.staticData.push_back(((float)std::rand() / (float)RAND_MAX - 0.05f) * 5.f);
			particles.staticData.push_back(((float)std::rand() / (float)RAND_MAX - 0.5f) * 1.f);
			particles.staticData.push_back(0.f);
			particles.staticData.push_back(0.f);

			particles.color.push_back(255 * (float)std::rand() / (float)RAND_MAX);
			particles.color.push_back(255 * (float)std::rand() / (float)RAND_MAX);
			particles.color.push_back(255 * (float)std::rand() / (float)RAND_MAX);
			particles.color.push_back(128);
		}
		renderer->addParticles(particles);
*/
	}
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
