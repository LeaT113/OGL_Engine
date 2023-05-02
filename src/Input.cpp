#include <iostream>
#include "Input.hpp"


Input::Input() = default;

void Input::OnKeyChanged(int key, int action)
{
	if(key < 0 || key > GLFW_KEY_LAST)
		return;
	if(!(action == GLFW_PRESS || action == GLFW_RELEASE))
		return;

	keyState[key] = action == GLFW_PRESS;
}
