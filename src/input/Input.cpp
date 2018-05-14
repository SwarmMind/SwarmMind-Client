#include <input/Input.h>

using namespace std;

Input::Input(GLFWwindow* window)
	: _window{window} 
{
	actionStatus[MoveRight] =	ActionStatus({ GLFW_KEY_D });
	actionStatus[MoveLeft] =	ActionStatus({ GLFW_KEY_A });
	actionStatus[MoveUp] =		ActionStatus({ GLFW_KEY_W });
	actionStatus[MoveDown] =	ActionStatus({ GLFW_KEY_S });

	actionStatus[ShootRight] =	ActionStatus({ GLFW_KEY_H });
	actionStatus[ShootLeft] =	ActionStatus({ GLFW_KEY_F });
	actionStatus[ShootUp] =		ActionStatus({ GLFW_KEY_T });
	actionStatus[ShootDown] =	ActionStatus({ GLFW_KEY_G });

	actionStatus[SelectUnit1] = ActionStatus({ GLFW_KEY_1 });
	actionStatus[SelectUnit2] = ActionStatus({ GLFW_KEY_2 });
	actionStatus[SelectUnit3] = ActionStatus({ GLFW_KEY_3 });
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
	for (auto& actionPair: actionStatus)
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
	for (int keyCode: glfwKeys)
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
