#include <imgui/imgui.h>
#include <imgui/opengl3_example/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
#include <input/Input.h>

using namespace std;

Input::Input(GLFWwindow* window)
	: _window{window} 
	, keyToStatus{ { Key::W, ActionStatus()}, { Key::A, ActionStatus()}, { Key::S, ActionStatus()}, { Key::D, ActionStatus()} }
{}

Input::~Input() 
{

}

bool Input::isActionPressed(Key keycode)
{
	if (glfwGetKey(_window, keycode) == GLFW_PRESS) 
	{
		return true;
	}
	return false;
}

void Input::updateKeyStatus()
{
	for each (auto key in keyToStatus) {
		checkPressedKey(key);
	}
}

void Input::checkPressedKey(pair<Key, ActionStatus> key)
{
	if (isActionPressed(key.first)) {
		if (keyToStatus.at(key.first).isJustPressed) {
			keyToStatus.at(key.first).isPressed = true;
			keyToStatus.at(key.first).isJustPressed = false;
		}
		else {
			keyToStatus.at(key.first).isJustPressed = true;
		}
	}
	else {
		keyToStatus.at(key.first).isPressed = false;
		keyToStatus.at(key.first).isJustPressed = false;
	}
}

map<Key, ActionStatus> Input::getKeyStatus()
{
	return keyToStatus;
}

