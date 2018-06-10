#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <map>
#include <vector>
#include <game/Camera.h>
#include <glm/fwd.hpp>

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
	MoveCameraRight,
	MoveCameraLeft,
	MoveCameraUp,
	MoveCameraDown
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
	Input(GLFWwindow* window, Camera* camera);
	~Input();

	bool isActionReleased(Action action);
	bool isActionPressed(Action action);
	bool isActionJustPressed(Action action);
	bool isActionJustReleased(Action action);

	void update(double deltaTime);
	
	glm::vec2 screenToWorldCoordinate(glm::vec2 mousePosition);
	glm::vec2 mousePositionInWorld();

private:
	map<Action, ActionStatus> actionStatus;
	GLFWwindow* _window;
	Camera* _camera;

	double _xMousePosition, _yMousePosition;
	glm::vec2 getMousePosition();
	
	void updateAction(Action action);

	void setMousePosition();
	bool isAnyKeyPressed(std::vector<int> glfwKeys);
	bool isMousePressed(std::vector<int> glfwKeys);	

	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void moveCamera(double deltaTime);
	float cameraMovementSpeed();
};
