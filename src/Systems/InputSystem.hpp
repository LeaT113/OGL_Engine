#ifndef OGL_ENGINE_INPUTSYSTEM_HPP
#define OGL_ENGINE_INPUTSYSTEM_HPP
#include <GLFW/glfw3.h>


class InputSystem
{
public:
	InputSystem();

	void OnKeyChanged(int key, int action);
    bool IsKeyPressed(int key) const;

    void OnMouseButtonChanged(int button, int action);
    bool IsMouseButtonPressed(int button) const;

    void OnMouseMoved(double xPos, double yPos);
    void RestartRelativeMouse();
    void GetRelativeMouse(float& movementX, float& movementY) const;

private:
	bool _keyState[GLFW_KEY_LAST + 1];
    bool _mouseButtonState[GLFW_MOUSE_BUTTON_LAST + 1];
    double _mouseStartX, _mouseStartY;
    double _mouseCurrentX, _mouseCurrentY;

    inline bool IsKeyValid(int key) const;
    inline bool IsMouseButtonValid(int button) const;
};


#endif //OGL_ENGINE_INPUTSYSTEM_HPP
