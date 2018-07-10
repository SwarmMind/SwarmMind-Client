#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <game/Camera.h>
#include <glm/vec2.hpp>

using namespace std;

enum Action {
	Move,
    MoveDirect,
	Shoot,
    ShootDirect,
	
    SelectUnit1,
	SelectUnit2,
	SelectUnit3,
	SelectUnit,
	
    Debug,
    Audio,
	
    MoveCameraRight,
	MoveCameraLeft,
	MoveCameraUp,
	MoveCameraDown,
    MoveCamera
};

struct KeyBinding {
    int m_key;
    int m_modifiers;

    KeyBinding(int key) : m_key{key}, m_modifiers{0} {}
    KeyBinding(int key, int modifiers) : m_key{key}, m_modifiers{modifiers} {}
};

typedef struct ActionStatus {

	ActionStatus(vector<KeyBinding> keys);
	ActionStatus();

	std::vector<KeyBinding> glfwKeys;
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
	unordered_map<Action, ActionStatus> actionStatus;
	GLFWwindow* _window;
	Camera* _camera;

    glm::vec2 m_mouseMoveClickPosition;
	double _xMousePosition, _yMousePosition;
	glm::vec2 getMousePosition();
	
	void updateAction(Action action);

	void setMousePosition();
	bool isAnyKeyPressed(std::vector<KeyBinding> glfwKeys);
	bool isMousePressed(std::vector<KeyBinding> glfwKeys);

	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void moveCamera(double deltaTime);
	float cameraMovementSpeed();
};
