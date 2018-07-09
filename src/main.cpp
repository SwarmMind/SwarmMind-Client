#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <glbinding/gl41core/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/callbacks.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <game/Game.h>

void initializeOpenGL() {
    glbinding::Binding::initialize(); 
    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, { "glGetError"   }); 
    glbinding::setAfterCallback([](const glbinding::FunctionCall &) 
    { 
        const auto error = glGetError(); 
        if (error != 0) 
            std::cout << "error: " << std::hex << error << std::endl; 
    });
}


int main(void)
{
	if (!glfwInit()) 
	{
		return EXIT_FAILURE;
	}
    initializeOpenGL();

    try {
        Game game;
        game.loop();
    } catch (std::runtime_error e) {
        std::cerr << e.what() << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
