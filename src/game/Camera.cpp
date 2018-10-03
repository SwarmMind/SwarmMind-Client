#include <imgui/imgui.h>
#include <game/Camera.h>


Camera::Camera(GLFWwindow* window) 
	: m_window{ window }
{

}

Camera::Camera(GLFWwindow* window, float x, float y, float height)
    : m_window{ window }
    , m_pos{ x, y }
    , m_height{ height } {

}

Camera::~Camera() 
{

}

void Camera::scroll(const Orientation orientation, const bool reverse, const double deltaTime) {
	const float speed = static_cast<float>(deltaTime) * m_height * (reverse ? (-1.0f) : 1.0f);

	switch (orientation) {
	case Orientation::Horizontal:
		m_pos.x += speed;
		break;
	case Orientation::Vertical:
		m_pos.y += speed;
		break;
	}
}

void Camera::move(const glm::vec2 direction) {
	m_pos += direction;
}

void Camera::setPosition(const float x, const float y)
{
	m_pos.x = x;
	m_pos.y = y;
}

void Camera::setHeight(float height)
{
	m_height = height;
}

float Camera::width() const noexcept
{
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(m_window, &bufferWidth, &bufferHeight);
	return (static_cast<float>(bufferWidth) / static_cast<float>(bufferHeight)) * m_height;
}

float Camera::height() const noexcept
{
	return m_height;
}

glm::vec2 Camera::position() const noexcept
{
	return m_pos;
}

glm::vec2 Camera::extent() const noexcept
{
	return glm::vec2{ width(), m_height };
}

