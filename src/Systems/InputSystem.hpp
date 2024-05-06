#ifndef OGL_ENGINE_INPUTSYSTEM_HPP
#define OGL_ENGINE_INPUTSYSTEM_HPP
#include <GLFW/glfw3.h>
#include "../Core/HybridSingleton.hpp"

class InputSystem : public HybridSingleton<InputSystem>
{
public:
	InputSystem();

	void OnKeyChanged(int key, int action);
    bool IsKeyPressed(int key) const;
	static bool GetKeyPress(int key);

    void OnMouseButtonChanged(int button, int action);
    bool IsMouseButtonPressed(int button) const;

	// Mouse
    void OnMouseMoved(double xPos, double yPos);
    void RestartRelativeMouse();
    void GetRelativeMouse(float& movementX, float& movementY) const;
	static std::pair<unsigned int, unsigned int> GetMousePosition();
	static bool GetMouseButtonPress(int button);

private:
	bool _keyState[GLFW_KEY_LAST + 1];
	bool _keyPress[GLFW_KEY_LAST + 1];
    bool _mouseButtonState[GLFW_MOUSE_BUTTON_LAST + 1];
    bool _mouseButtonPress[GLFW_MOUSE_BUTTON_LAST + 1];
    double _mouseStartX, _mouseStartY;
    double _mouseCurrentX, _mouseCurrentY;

    inline bool IsKeyValid(int key) const;
    inline bool IsMouseButtonValid(int button) const;
};


#endif //OGL_ENGINE_INPUTSYSTEM_HPP
