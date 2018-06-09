#include <input/Input.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <iostream>

using namespace std;

Input::Input(GLFWwindow* window, Camera* camera)
	: _window{window} 
	, _camera{camera}
{
	actionStatus[MoveRight]  =	ActionStatus({ GLFW_KEY_D });
	actionStatus[MoveLeft]   =	ActionStatus({ GLFW_KEY_A });
	actionStatus[MoveUp]  	 =	ActionStatus({ GLFW_KEY_W });
	actionStatus[MoveDown]   =	ActionStatus({ GLFW_KEY_S });
	actionStatus[Move]       = ActionStatus({ GLFW_MOUSE_BUTTON_LEFT });

	actionStatus[ShootRight] =	ActionStatus({ GLFW_KEY_H });
	actionStatus[ShootLeft]  =	ActionStatus({ GLFW_KEY_F });
	actionStatus[ShootUp]    = 		ActionStatus({ GLFW_KEY_T });
	actionStatus[ShootDown]  =	ActionStatus({ GLFW_KEY_G });
	actionStatus[Shoot]      = ActionStatus({ GLFW_MOUSE_BUTTON_RIGHT });

	actionStatus[SelectUnit1] = ActionStatus({ GLFW_KEY_1 });
	actionStatus[SelectUnit2] = ActionStatus({ GLFW_KEY_2 });
	actionStatus[SelectUnit3] = ActionStatus({ GLFW_KEY_3 });
	actionStatus[SelectUnit]  = ActionStatus({ GLFW_MOUSE_BUTTON_LEFT });

	actionStatus[MoveCameraRight] = ActionStatus({ GLFW_KEY_RIGHT });
	actionStatus[MoveCameraLeft]  = ActionStatus({ GLFW_KEY_LEFT });
	actionStatus[MoveCameraUp]    = ActionStatus({ GLFW_KEY_UP });
	actionStatus[MoveCameraDown]  = ActionStatus({ GLFW_KEY_DOWN });

	actionStatus[Debug] = ActionStatus({ GLFW_KEY_PERIOD });

	glfwSetWindowUserPointer(_window, this);
	auto scrollLambda = [](GLFWwindow* window, double xoffset, double yoffset) {
		static_cast<Input*>(glfwGetWindowUserPointer(window))->scrollCallback(window, xoffset, yoffset);
		
	};
	glfwSetScrollCallback(_window, scrollLambda);
}

Input::~Input() 
{

}

bool Input::isActionReleased(Action action)
{
	return !this->isActionPressed(action);
}

bool Input::isActionPressed(Action action)
{
	return actionStatus[action].isPressed;
}

bool Input::isActionJustPressed(Action action)
{
	return actionStatus[action].isJustPressed;
}

bool Input::isActionJustReleased(Action action)
{
	return actionStatus[action].isJustReleased;
}

void Input::update()
{
	setMousePosition();
	moveCamera();
	
	for (auto& actionPair: actionStatus)
	{
		this->updateAction(actionPair.first);
	}
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
{
	if (yoffset == -1) { // scrolling up means zooming in
		_camera->setCamera(_camera->getX(), _camera->getY(), _camera->getHeight()*1.3);
	}
	else { // scrolling down means zooming out
		_camera->setCamera(_camera->getX(), _camera->getY(), _camera->getHeight()*0.7);
	}
}

void Input::moveCamera() {

	if (isActionJustPressed(MoveCameraDown)) {
		_camera->setCamera(_camera->getX(), _camera->getY() - 1, _camera->getHeight());
	}
	if (isActionJustReleased(MoveCameraUp)) {
		_camera->setCamera(_camera->getX(), _camera->getY() + 1, _camera->getHeight());
	}
	if (isActionJustReleased(MoveCameraRight)) {
		_camera->setCamera(_camera->getX() + 1, _camera->getY(), _camera->getHeight());
	}
	if (isActionJustReleased(MoveCameraLeft)) {
		_camera->setCamera(_camera->getX() - 1, _camera->getY(), _camera->getHeight());
	}
}

void Input::updateAction(Action action)
{
	ActionStatus& status = actionStatus[action];

	if (isAnyKeyPressed(status.glfwKeys) || isMousePressed(status.glfwKeys)) {
		status.isJustPressed = !status.isPressed;
		status.isPressed = true;
		status.isJustReleased = false;
	} else {
		status.isJustPressed = false;
		status.isJustReleased = status.isPressed;
		status.isPressed = false;
	}
}

bool Input::isAnyKeyPressed(std::vector<int> glfwKeys)
{
	bool imGuiConsumesInput = ImGui::GetIO().WantCaptureKeyboard;
	for (int keyCode: glfwKeys)
	{
		if (glfwGetKey(_window, keyCode) == GLFW_PRESS && !imGuiConsumesInput)
		{
			return true;
		}
	}
	return false;
}

bool Input::isMousePressed(std::vector<int> glfwKeys) {
	bool imGuiConsumesInput = ImGui::GetIO().WantCaptureMouse;

	for (int mouseCode : glfwKeys)
	if (glfwGetMouseButton(_window, mouseCode) == GLFW_PRESS && !imGuiConsumesInput) {
			return true;
	}
	return false;
}

void Input::setMousePosition() {
	glfwGetCursorPos(_window, &_xMousePosition, &_yMousePosition);
}

glm::vec2 Input::getMousePosition() {
	float xFloatMousePosition = (float)_xMousePosition;
	float yFloatMousePosition = (float)_yMousePosition;
	return glm::vec2(xFloatMousePosition, yFloatMousePosition);
}

glm::vec2 Input::mousePositionInWorld() {
	return screenToWorldCoordinate(getMousePosition());
}

glm::vec2 Input::screenToWorldCoordinate(glm::vec2 mousePosition) {

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(_window, &bufferWidth, &bufferHeight);
	
	// set origin to lower left corner
	mousePosition.y = bufferHeight - mousePosition.y;

	// normalize
	mousePosition.x = mousePosition.x / bufferWidth;
	mousePosition.y = mousePosition.y / bufferHeight; 

	// unified screen coordinate
	mousePosition.x = mousePosition.x * 2 - 1;
	mousePosition.y = mousePosition.y * 2 - 1; 
	
	mousePosition.x = mousePosition.x * _camera->getWidth() + _camera->getX();
	mousePosition.y = mousePosition.y * _camera->getHeight() + _camera->getY();	

	return glm::vec2(mousePosition.x, mousePosition.y);
}

ActionStatus::ActionStatus(vector<int> keys) 
	: glfwKeys{ keys }
{}

ActionStatus::ActionStatus()
{}

