#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class ImGuiRenderer
{
	public:
		ImGuiRenderer(GLFWwindow* window);
		~ImGuiRenderer();
		
		void preRender();
		void render();
};

