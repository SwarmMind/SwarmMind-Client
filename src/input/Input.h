#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <map>

using namespace std;

enum Key {
	W = GLFW_KEY_W,
	A = GLFW_KEY_A,
	S = GLFW_KEY_S,
	D = GLFW_KEY_D
};

typedef struct ActionStatus {
	bool isPressed = false;
	bool isJustPressed = false;
	bool isJustReleased = !isJustPressed;
} ActionStatus;

class Input
{
public:
	Input(GLFWwindow* window);
	~Input();

	map<Key, ActionStatus> getKeyStatus();

private:
	map<Key, ActionStatus> keyToStatus;
	GLFWwindow* _window;

	bool isActionPressed(Key keycode);
	void updateKeyStatus();
	void checkPressedKey(pair<Key, ActionStatus>);
};
