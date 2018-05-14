#include <input/Input.h>

using namespace std;

Input::Input(GLFWwindow* window)
	: _window{window} 
{
	actionStatus[MoveRight] =	ActionStatus({ GLFW_KEY_D });
	actionStatus[MoveLeft] =	ActionStatus({ GLFW_KEY_A });
	actionStatus[MoveUp] =		ActionStatus({ GLFW_KEY_W });
	actionStatus[MoveDown] =	ActionStatus({ GLFW_KEY_S });
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

void Input::update(double deltaTime)
{
	for each (auto& actionPair in actionStatus)
	{
		this->updateAction(actionPair.first);
	}
}

void Input::updateAction(Action action)
{
	ActionStatus& status = actionStatus[action];

	if (isAnyKeyPressed(status.glfwKeys)) {
		status.isJustPressed = !status.isPressed;
		status.isPressed = true;
		status.isJustReleased = false;
	}
	else {
		status.isJustPressed = false;
		status.isJustReleased = status.isPressed;
		status.isPressed = false;
	}
}

bool Input::isAnyKeyPressed(std::vector<int> glfwKeys)
{
	for each (int keyCode in glfwKeys)
	{
		if (glfwGetKey(_window, keyCode) == GLFW_PRESS)
		{
			return true;
		}
	}
	return false;
}

ActionStatus::ActionStatus(vector<int> keys) 
	: glfwKeys{ keys }
{}

ActionStatus::ActionStatus()
{}
