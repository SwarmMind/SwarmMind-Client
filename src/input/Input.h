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
	MoveDown
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


	bool isActionPressed(Action action);
	bool isActionJustPressed(Action action);
	bool isActionJustReleased(Action action);

	void update(double deltaTime); //Time is not needed, but added for consistency with rest of game loop

private:
	map<Action, ActionStatus> actionStatus;
	GLFWwindow* _window;

	void updateAction(Action action);
	bool isAnyKeyPressed(std::vector<int> glfwKeys);
};
