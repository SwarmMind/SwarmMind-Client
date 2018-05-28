#include <imgui/imgui.h>
#include <game/Camera.h>


Camera::Camera(GLFWwindow* window) 
	: _window{ window }
{

}

Camera::~Camera() 
{

}

void Camera::setCamera(float x, float y, float height)
{
	_x = x;
	_y = y;
	_height = height;
}

float Camera::getWidth() {
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(_window, &bufferWidth, &bufferHeight);
	_width = (static_cast<float>(bufferWidth) / static_cast<float>(bufferHeight)) * _height;

	return _width;
}

float Camera::getHeight() {
	return _height;
}

float Camera::getX() {
	return _x;
}

float Camera::getY() {
	return _y;
}