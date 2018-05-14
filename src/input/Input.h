#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <map>
#include <vector>

using namespace std;

enum Action {
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	SelectUnit1,
	SelectUnit2,
	SelectUnit3
};

typedef struct ActionStatus {

	ActionStatus(vector<int> keys);
	ActionStatus();

	std::vector<int> glfwKeys;
	bool isPressed = false;
	bool isJustPressed = false;
	bool isJustReleased = false;
} ActionStatus;

class Input
{
public:
	Input(GLFWwindow* window);
	~Input();

	bool isActionReleased(Action action);
	bool isActionPressed(Action action);
	bool isActionJustPressed(Action action);
	bool isActionJustReleased(Action action);

	void update();

private:
	map<Action, ActionStatus> actionStatus;
	GLFWwindow* _window;

	void updateAction(Action action);
	bool isAnyKeyPressed(std::vector<int> glfwKeys);
};
