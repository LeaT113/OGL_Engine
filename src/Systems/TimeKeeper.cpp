#include <iostream>
#include <iomanip>
#include "TimeKeeper.hpp"

TimeKeeper::TimeKeeper(): _startupTime(std::chrono::steady_clock::now())
{
	Update();
}

void TimeKeeper::Update()
{
	TimePoint currentTime = std::chrono::steady_clock::now();

	_timeSinceStartup = std::chrono::duration<double>(currentTime - _startupTime).count();

	_deltaTime = std::chrono::duration<double>(currentTime - _lastUpdateTime).count();

	_lastUpdateTime = currentTime;

	if(_logging)
		std::cout << std::setprecision(1) << std::fixed << TimeSinceStartup() << "s | " << DeltaTime() * 1000 << "ms | " << 1 / DeltaTime() << "FPS" << std::endl;
}

double TimeKeeper::TimeSinceStartup()
{
	return Instance()._timeSinceStartup;
}

double TimeKeeper::DeltaTime()
{
	return Instance()._deltaTime;
}

void TimeKeeper::EnableLogging(bool enable)
{
	_logging = enable;
}






