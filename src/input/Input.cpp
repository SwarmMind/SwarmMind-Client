#include <input/Input.h>
#include <imgui/imgui.h>
#include <iostream>

using namespace std;

Input::Input(GLFWwindow* window)
	: _window{window} 
{
	actionStatus[MoveRight] =	ActionStatus({ GLFW_KEY_D });
	actionStatus[MoveLeft] =	ActionStatus({ GLFW_KEY_A });
	actionStatus[MoveUp] =		ActionStatus({ GLFW_KEY_W });
	actionStatus[MoveDown] =	ActionStatus({ GLFW_KEY_S });
	actionStatus[Move] = ActionStatus({ GLFW_MOUSE_BUTTON_LEFT });

	actionStatus[ShootRight] =	ActionStatus({ GLFW_KEY_H, GLFW_KEY_RIGHT });
	actionStatus[ShootLeft] =	ActionStatus({ GLFW_KEY_F, GLFW_KEY_LEFT });
	actionStatus[ShootUp] =		ActionStatus({ GLFW_KEY_T, GLFW_KEY_UP });
	actionStatus[ShootDown] =	ActionStatus({ GLFW_KEY_G, GLFW_KEY_DOWN });
	actionStatus[Shoot] = ActionStatus({ GLFW_MOUSE_BUTTON_RIGHT });

	actionStatus[SelectUnit1] = ActionStatus({ GLFW_KEY_1 });
	actionStatus[SelectUnit2] = ActionStatus({ GLFW_KEY_2 });
	actionStatus[SelectUnit3] = ActionStatus({ GLFW_KEY_3 });
	actionStatus[SelectUnit] = ActionStatus({ GLFW_MOUSE_BUTTON_MIDDLE });

	actionStatus[Debug] = ActionStatus({ GLFW_KEY_PERIOD });
	//actionStatus[ChooseDirection] = ActionStatus({ GLFW_MOUSE_BUTTON_LEFT });
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
	for (auto& actionPair: actionStatus)
	{
		this->updateAction(actionPair.first);
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

bool Input::isMousePressed(std::vector<int> glfwKeys) { // should it become one function?
	bool imGuiConsumesInput = ImGui::GetIO().WantCaptureMouse;

	for (int mouseCode : glfwKeys)
	if (glfwGetMouseButton(_window, mouseCode) == GLFW_PRESS && !imGuiConsumesInput) {
			return true;
	}
	return false;
}

/*
MousePosition Input::getMousePosition() {
	MousePosition position; 
	glfwGetCursorPos(_window, &position.xMousePosition, &position.yMousePosition);
	
	return position;
}
*/

void Input::setMousePosition() {
	glfwGetCursorPos(_window, &xMousePosition, &yMousePosition);
}

double Input::getMousePosition(string coordinate) {
	if (coordinate == "x") {
		return xMousePosition;
	} else {
		return yMousePosition;
	}
}

ActionStatus::ActionStatus(vector<int> keys) 
	: glfwKeys{ keys }
{}

ActionStatus::ActionStatus()
{}

