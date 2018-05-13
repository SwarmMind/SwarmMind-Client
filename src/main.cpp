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

void update(double time)
{

}

OpenGLRenderer* renderer;
ImGuiRenderer* imguiRenderer;
Sprite* gridSprite;
Sprite* monsterSprite;
Sprites* sprites;
Textures* textures;
Input* input;

void render(GLFWwindow* window, float timeElapsed)
{

	imguiRenderer->preRender();
	renderer->preDraw();
	double time = glfwGetTime();
	for (size_t x = 0; x < 60; x++)
	{
		for (size_t y = 0; y < 30; y++)
		{
			renderer->drawSprite(x, y, 0.3, 1, 1, gridSprite);
			if (x % 2)
				renderer->drawSprite(x, y, 0.5, 1, 1, monsterSprite);
		}
	}
	double elapsed = glfwGetTime() - time;
	renderer->draw();
	ImGui::Text("renderTime: %f", elapsed * 1000);

	{
		static bool stayOpen = true;
		static float uiSize = 1.0f;
		static std::deque<float> frameTimeQueue;
		static int fpsAverageTime = 120;

		while (frameTimeQueue.size() >= fpsAverageTime)
		{
			frameTimeQueue.pop_front();
		}
		frameTimeQueue.push_back(timeElapsed);

		if(stayOpen)
		{
			ImGui::Begin("My First Window", &stayOpen, ImGuiWindowFlags_AlwaysAutoResize); 
			{
				float fps = 0;
				for (float frameTime : frameTimeQueue)
				{
					fps += frameTime;
				}
				fps = (float) fpsAverageTime / fps;
				ImGui::Text("Average FPS: %f", fps);
				ImGui::DragFloat("UI scale", &uiSize, 0.001f, 0.f, 5.f);
				ImGui::GetIO().FontGlobalScale = uiSize;
			}
			ImGui::End();
		}
		else
		{
			ShowExampleAppFixedOverlay(nullptr);
		}
	}

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
	textures = new Textures();
	sprites = new Sprites(*textures);
	gridSprite = sprites->get(GridBlock);
	monsterSprite = sprites->get(Monster);
	imguiRenderer = new ImGuiRenderer(window);
	input = new Input(window);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 20);
	io.FontGlobalScale = 1.0f;
	ImGui::StyleColorsDark();

	renderer->setCamera(30, 15, 15);

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

	delete sprites;
	delete textures;
	delete renderer;
	delete imguiRenderer;

	shutdown(0);
}