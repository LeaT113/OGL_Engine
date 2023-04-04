#include "Time.hpp"

Time::Time(): _startupTime(std::chrono::steady_clock::now())
{
	Update();
}

double Time::TimeSinceStartup() const
{
	return _timeSinceStartup;
}

double Time::DeltaTime() const
{
	return _deltaTime;
}

void Time::Update()
{
	TimePoint currentTime = std::chrono::steady_clock::now();

	_timeSinceStartup = std::chrono::duration<double>(currentTime - _startupTime).count();

	_deltaTime = std::chrono::duration<double>(currentTime - _lastUpdateTime).count();

	_lastUpdateTime = currentTime;
}






