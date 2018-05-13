#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <queue>

#include <renderer/OpenGLRenderer.h>
#include <renderer/ImGuiRenderer.h>
#include <imgui/imgui.h>
#include <imgui/opengl3_example/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_demo.cpp>
#include <renderer/Sprite.h>
#include <renderer/Texture.h>
#include <renderer/Textures.h>
#include <renderer/Sprites.h>
#include <input/Input.h>
#include <thread>
#include <gamestate/Gamestate.h>
#include <gamestate/Networker.h>

using namespace gl;
using namespace std;

void shutdown(int errorCode = 0)
{
	glfwTerminate();
	exit(errorCode);
}

GLFWwindow* createGLFWWindow()
{
	GLFWwindow* window;
	
	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
	if (!window)
	{
		cout << "GLFW window could not be created! Please check your graphics driver. OpenGL v4.0 is required!\n";
		shutdown(-1);
	}

	return window;
}

void initializeOpenGL(GLFWwindow* window) 
{
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

void processInputs()
{
	/* Poll for and process events */
	glfwPollEvents();
}

OpenGLRenderer* renderer;
ImGuiRenderer* imguiRenderer;
Gamestate* gameState;

void update(double time)
{
	gameState->update(time);
}

<<<<<<< HEAD
OpenGLRenderer* renderer;
ImGuiRenderer* imguiRenderer;
Sprite* gridSprite;
Sprite* monsterSprite;
Sprites* sprites;
Textures* textures;
Input* input;

=======
>>>>>>> 2032c67d40af23984e502b9775660e4982f8ef82
void render(GLFWwindow* window, float timeElapsed)
{

	imguiRenderer->preRender();
	renderer->preDraw();
	
	gameState->draw(*renderer);

	renderer->draw();
	imguiRenderer->render();
	glfwSwapBuffers(window);

}



int main(void)
{
	if (!glfwInit()) 
	{
		return -1;
	}
	
	GLFWwindow* window = createGLFWWindow();
	initializeOpenGL(window);

	renderer = new OpenGLRenderer(window);
	imguiRenderer = new ImGuiRenderer(window);
	input = new Input(window);

	Networker localHost("127.0.0.1");
	gameState = new Gamestate(localHost);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 20);
	io.FontGlobalScale = 1.0f;
	ImGui::StyleColorsDark();

	renderer->setCamera(15, 15, 15);

	/* Loop until the user closes the window */
	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		double current = glfwGetTime();
		double elapsed = current - lastTime;
		processInputs();
		//input handling
		input->updateKeyStatus();
		update(elapsed);
		render(window, elapsed);
		lastTime = current;
	}

	delete renderer;
	delete imguiRenderer;
	delete gameState;

	shutdown(0);
}