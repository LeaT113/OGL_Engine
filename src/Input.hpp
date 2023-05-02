#ifndef OGL_ENGINE_INPUT_HPP
#define OGL_ENGINE_INPUT_HPP
#include <GLFW/glfw3.h>


class Input
{
public:
	Input();

	void OnKeyChanged(int key, int action);

private:
	bool keyState[GLFW_KEY_LAST+1];
};


#endif //OGL_ENGINE_INPUT_HPP
