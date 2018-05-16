#include <game/Game.h>
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
#include <input/Input.h>
#include <functional>
#include <menu/MainMenuState.h>
#include <menu/ConnectedState.h>


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

Game::Game(){
    createWindow();
    initializeOpenGL();

    renderer = new OpenGLRenderer(window);
	renderer->setCamera(5, 5, 10);
    imguiRenderer = new ImGuiRenderer(window);
    input = new Input(window);

	initializeImGui();

	menu = std::make_unique<MainMenuState>(this);
}


void Game::connectTo(std::string address, unsigned int port)
{
	menu = std::make_unique<ConnectedState>(*input, address, port);
}

void Game::initializeImGui()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 20);
	io.FontGlobalScale = 1.0f;
	ImGui::StyleColorsDark();
}

Game::~Game() {
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

    renderer->draw();
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
