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
	Move,
	ShootLeft,
	ShootRight,
	ShootUp,
	ShootDown,
	Shoot,
	SelectUnit1,
	SelectUnit2,
	SelectUnit3,
	SelectUnit,
	Debug,
	ChooseDirection
};

typedef struct ActionStatus {

	ActionStatus(vector<int> keys);
	ActionStatus();

	std::vector<int> glfwKeys;
	bool isPressed = false;
	bool isJustPressed = false;
	bool isJustReleased = false;

} ActionStatus;

/*
typedef struct MousePosition {

	double xMousePosition;
	double yMousePosition;

} MousePosition;
*/

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
	double getMousePosition(string coordinate);
	//MousePosition getMousePosition();

private:
	map<Action, ActionStatus> actionStatus;
	GLFWwindow* _window;
	double xMousePosition, yMousePosition;
	
	void updateAction(Action action);
	void setMousePosition();
	bool isAnyKeyPressed(std::vector<int> glfwKeys);
	bool isMousePressed(std::vector<int> glfwKeys);
};
