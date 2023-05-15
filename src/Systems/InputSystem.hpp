#ifndef OGL_ENGINE_INPUTSYSTEM_HPP
#define OGL_ENGINE_INPUTSYSTEM_HPP
#include <GLFW/glfw3.h>


class InputSystem
{
public:
	InputSystem();

	void OnKeyChanged(int key, int action);

private:
	bool keyState[GLFW_KEY_LAST+1];
};


#endif //OGL_ENGINE_INPUTSYSTEM_HPP
