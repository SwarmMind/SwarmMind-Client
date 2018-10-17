#include <input/Input.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

Input::Input(GLFWwindow* window, Camera* camera)
	: m_window{window} 
	, m_camera{camera}
{
	m_actionStatus[Move]       =  ActionStatus({ GLFW_MOUSE_BUTTON_LEFT });
    m_actionStatus[MoveDirect] =  ActionStatus({ KeyBinding { GLFW_MOUSE_BUTTON_LEFT, GLFW_MOD_SHIFT } });

	m_actionStatus[Shoot]       = ActionStatus({ GLFW_MOUSE_BUTTON_RIGHT });
    m_actionStatus[ShootDirect] = ActionStatus({ KeyBinding { GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOD_SHIFT } });

	m_actionStatus[SelectUnit1] = ActionStatus({ GLFW_KEY_1 });
	m_actionStatus[SelectUnit2] = ActionStatus({ GLFW_KEY_2 });
	m_actionStatus[SelectUnit3] = ActionStatus({ GLFW_KEY_3 });
	m_actionStatus[SelectUnit]  = ActionStatus({ GLFW_MOUSE_BUTTON_LEFT });

	m_actionStatus[MoveCameraRight] = ActionStatus({ GLFW_KEY_D, GLFW_KEY_RIGHT });
	m_actionStatus[MoveCameraLeft]  = ActionStatus({ GLFW_KEY_A, GLFW_KEY_LEFT });
	m_actionStatus[MoveCameraUp]    = ActionStatus({ GLFW_KEY_W, GLFW_KEY_UP });
	m_actionStatus[MoveCameraDown]  = ActionStatus({ GLFW_KEY_S, GLFW_KEY_DOWN });
    m_actionStatus[MoveCamera] = ActionStatus({ GLFW_MOUSE_BUTTON_MIDDLE });

	m_actionStatus[Debug] = ActionStatus({ GLFW_KEY_PERIOD });

	glfwSetWindowUserPointer(m_window, this);
	auto scrollLambda = [](GLFWwindow* window, double xoffset, double yoffset) {
		static_cast<Input*>(glfwGetWindowUserPointer(window))->scrollCallback(window, xoffset, yoffset);		
	};
	glfwSetScrollCallback(m_window, scrollLambda);
}

Input::~Input() 
{
	glfwSetScrollCallback(m_window, nullptr);
}

bool Input::isActionReleased(Action action)
{
	return !this->isActionPressed(action);
}

bool Input::isActionPressed(Action action)
{
	return m_actionStatus[action].m_isPressed;
}

bool Input::isActionJustPressed(Action action)
{
	return m_actionStatus[action].m_isJustPressed;
}

bool Input::isActionJustReleased(Action action)
{
	return m_actionStatus[action].m_isJustReleased;
}

void Input::update(double deltaTime)
{
	setMousePosition();
	moveCamera(deltaTime);
	
	for (auto& actionPair: m_actionStatus)
	{
		this->updateAction(actionPair.first);
	}
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
{
    m_camera->setHeight(std::max(2.0f, std::min(m_camera->height() - static_cast<float>(yoffset), 20.0f)));
}

void Input::moveCamera(double deltaTime) 
{
	if (isActionPressed(MoveCameraDown)) {
		m_camera->scroll(Orientation::Vertical, true, deltaTime);
	}
	if (isActionPressed(MoveCameraUp)) {
		m_camera->scroll(Orientation::Vertical, false, deltaTime);
	}
	if (isActionPressed(MoveCameraRight)) {
		m_camera->scroll(Orientation::Horizontal, false, deltaTime);
	}
	if (isActionPressed(MoveCameraLeft)) {
		m_camera->scroll(Orientation::Horizontal, true, deltaTime);
	}


    if (isActionJustPressed(MoveCamera))
    {
        m_mouseMoveClickPosition = mousePositionInWorld();
    }
    if (isActionPressed(MoveCamera))
    {
		m_camera->move(m_mouseMoveClickPosition - mousePositionInWorld());
    }
}

void Input::updateAction(Action action)
{
	ActionStatus& status = m_actionStatus[action];

	if (isAnyKeyPressed(status.m_glfwKeys) || isMousePressed(status.m_glfwKeys)) {
		status.m_isJustPressed = !status.m_isPressed;
		status.m_isPressed = true;
		status.m_isJustReleased = false;
	} else {
		status.m_isJustPressed = false;
		status.m_isJustReleased = status.m_isPressed;
		status.m_isPressed = false;
	}
}

bool Input::isAnyKeyPressed(std::vector<KeyBinding> glfwKeys)
{
	bool imGuiConsumesInput = ImGui::GetIO().WantCaptureKeyboard;
	for (KeyBinding keyCode: glfwKeys)
	{
		if (glfwGetKey(m_window, keyCode.m_key) == GLFW_PRESS &&
            ((keyCode.m_modifiers & GLFW_MOD_SHIFT) != 0) == ImGui::GetIO().KeyShift &&
            !imGuiConsumesInput)
		{
			return true;
		}
	}
	return false;
}

bool Input::isMousePressed(std::vector<KeyBinding> glfwKeys) {
	bool imGuiConsumesInput = ImGui::GetIO().WantCaptureMouse;

	for (KeyBinding mouseCode : glfwKeys) {
    	if (glfwGetMouseButton(m_window, mouseCode.m_key) == GLFW_PRESS &&
            ((mouseCode.m_modifiers & GLFW_MOD_SHIFT) != 0) == ImGui::GetIO().KeyShift &&
            !imGuiConsumesInput) {
	    		return true;
    	}
    }
	return false;
}

void Input::setMousePosition() {
	glfwGetCursorPos(m_window, &m_xMousePosition, &m_yMousePosition);
}

glm::vec2 Input::getMousePosition() const {
	return glm::vec2( static_cast<float>(m_xMousePosition), static_cast<float>(m_yMousePosition));
}

glm::vec2 Input::mousePositionInWorld() {
	return screenToWorldCoordinate(getMousePosition());
}

glm::vec2 Input::screenToWorldCoordinate(glm::vec2 mousePosition) {

	int bufferWidth, bufferHeight;
	glfwGetWindowSize(m_window, &bufferWidth, &bufferHeight);
	
	// set origin to lower left corner
	mousePosition.y = bufferHeight - mousePosition.y;

	// normalize
	mousePosition.x = mousePosition.x / bufferWidth;
	mousePosition.y = mousePosition.y / bufferHeight; 

	// unified screen coordinate
	mousePosition.x = mousePosition.x * 2 - 1;
	mousePosition.y = mousePosition.y * 2 - 1; 
	
	const auto pos = m_camera->position();
	mousePosition.x = mousePosition.x * m_camera->width() + pos.x;
	mousePosition.y = mousePosition.y * m_camera->height() + pos.y;	

	return glm::vec2(mousePosition.x, mousePosition.y);
}

ActionStatus::ActionStatus(vector<KeyBinding> keys) 
	: m_glfwKeys{ keys }
{}

ActionStatus::ActionStatus()
{}

