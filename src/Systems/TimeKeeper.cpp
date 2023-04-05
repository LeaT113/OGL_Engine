#include "TimeKeeper.hpp"

TimeKeeper::TimeKeeper(): _startupTime(std::chrono::steady_clock::now())
{
	Update();
}

double TimeKeeper::TimeSinceStartup() const
{
	return _timeSinceStartup;
}

double TimeKeeper::DeltaTime() const
{
	return _deltaTime;
}

void TimeKeeper::Update()
{
	TimePoint currentTime = std::chrono::steady_clock::now();

	_timeSinceStartup = std::chrono::duration<double>(currentTime - _startupTime).count();

	_deltaTime = std::chrono::duration<double>(currentTime - _lastUpdateTime).count();

	_lastUpdateTime = currentTime;
}






