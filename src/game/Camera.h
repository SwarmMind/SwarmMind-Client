#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Camera {
public:
	Camera(GLFWwindow* window);
	~Camera();

	void setCamera(float x, float y, float height);
	float getHeight();
	float getWidth();
	float getX();
	float getY();

private:
	GLFWwindow* _window;

	float _x;
	float _y;
	float _height;
};