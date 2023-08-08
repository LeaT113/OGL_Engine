#include <iostream>
#include "InputSystem.hpp"


InputSystem::InputSystem() = default;

void InputSystem::OnKeyChanged(int key, int action)
{
	if(!IsKeyValid(key))
        return;
	if(!(action == GLFW_PRESS || action == GLFW_RELEASE))
		return;

    _keyState[key] = action == GLFW_PRESS;
}

bool InputSystem::IsKeyPressed(int key) const
{
    if(!IsKeyValid(key))
        return false;

    return _keyState[key];
}


void InputSystem::OnMouseButtonChanged(int button, int action)
{
    if(!IsMouseButtonValid(button))
        return;

    if(!(action == GLFW_PRESS || action == GLFW_RELEASE))
        return;

    _mouseButtonState[button] = action == GLFW_PRESS;
}

bool InputSystem::IsMouseButtonPressed(int button) const
{
    if(!IsMouseButtonValid(button))
        return false;

    return _mouseButtonState[button];
}


void InputSystem::OnMouseMoved(double xPos, double yPos)
{
    _mouseCurrentX = xPos;
    _mouseCurrentY = yPos;
}

void InputSystem::RestartRelativeMouse()
{
    _mouseStartX = _mouseCurrentX;
    _mouseStartY = _mouseCurrentY;
}

void InputSystem::GetRelativeMouse(float &movementX, float &movementY) const
{
    movementX = _mouseCurrentX - _mouseStartX;
    movementY = _mouseCurrentY - _mouseStartY;
}


inline bool InputSystem::IsKeyValid(int key) const
{
    return key >= 0 && key <= GLFW_KEY_LAST;
}

bool InputSystem::IsMouseButtonValid(int button) const
{
    return button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST;
}
