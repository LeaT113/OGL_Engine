#include <iostream>
#include "InputSystem.hpp"


InputSystem::InputSystem() = default;

void InputSystem::OnKeyChanged(int key, int action)
{
	if(!IsKeyValid(key))
        return;
	if(!(action == GLFW_PRESS || action == GLFW_RELEASE))
		return;

	keyState[key] = action == GLFW_PRESS;
}

bool InputSystem::IsKeyPressed(int key) const
{
    if(!IsKeyValid(key))
        return false;

    return keyState[key];
}

inline bool InputSystem::IsKeyValid(int key) const
{
    return key > 0 && key <= GLFW_KEY_LAST;
}

void InputSystem::OnMouseMoved(double xPos, double yPos)
{
    mouseCurrentX = xPos;
    mouseCurrentY = yPos;
}

void InputSystem::RestartRelativeMouse()
{
    mouseStartX = mouseCurrentX;
    mouseStartY = mouseCurrentY;
}

void InputSystem::GetRelativeMouse(float &movementX, float &movementY) const
{
    movementX = mouseCurrentX - mouseStartX;
    movementY = mouseCurrentY - mouseStartY;
}
