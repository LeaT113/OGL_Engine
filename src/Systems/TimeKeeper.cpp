#include "TimeKeeper.hpp"
#include <GLFW/glfw3.h>

TimeKeeper::TimeKeeper()
{
	Update();
}

void TimeKeeper::Update()
{
	auto currentTime = glfwGetTime();

	_timeSinceStartup = currentTime;

	_deltaTime = currentTime - _lastUpdateTime;

	_lastUpdateTime = currentTime;

	if(_logging)
		std::cout << std::setprecision(1) << std::fixed << TimeSinceStartup() << "s | " << DeltaTime() * 1000 << "ms | " << 1 / DeltaTime() << "FPS" << std::endl;
}

double TimeKeeper::TimeSinceStartup()
{
	return glfwGetTime();
}

double TimeKeeper::DeltaTime()
{
	return Instance()._deltaTime;
}

void TimeKeeper::EnableLogging(bool enable)
{
	_logging = enable;
}






