#include <iostream>
#include <stdexcept>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <game/Game.h>

int main(void)
{
	if (!glfwInit()) 
	{
		return -1;
	}

    try {
        Game game;
        game.loop();
    } catch (std::runtime_error e) {
        std::cerr << e.what() << std::endl;
        glfwTerminate();
        return -1;
    }

    return 0;
}
