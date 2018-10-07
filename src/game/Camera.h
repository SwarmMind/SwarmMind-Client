#pragma once

#include <glm/vec2.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

enum class Orientation {
	Horizontal,
	Vertical
};

class Camera {
public:
	Camera(GLFWwindow* window);
    Camera(GLFWwindow* window, float x, float y, float height);
	~Camera();

	void setPosition(const float x, const float y);
	void setHeight(const float height);
	void scroll(const Orientation orientation, const bool reverse, const double deltaTime);
	void move(const glm::vec2 direction);

	float height() const noexcept;
	float width() const noexcept;
	glm::vec2 position() const noexcept;
	glm::vec2 extent() const noexcept;
	
private:
	GLFWwindow* m_window;

	glm::vec2 m_pos;
	float m_height;
};
