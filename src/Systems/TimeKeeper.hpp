#ifndef OGL_ENGINE_TIMEKEEPER_HPP
#define OGL_ENGINE_TIMEKEEPER_HPP
#include <chrono>

class TimeKeeper
{
public:
	double TimeSinceStartup() const;
	double DeltaTime() const;

	TimeKeeper();

	void Update();

private:
	using Duration = std::chrono::duration<long long, std::nano>;
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock, Duration>;

	const TimePoint _startupTime;
	TimePoint _lastUpdateTime;
	double _timeSinceStartup = 0;
	double _deltaTime = 0;
};


#endif //OGL_ENGINE_TIMEKEEPER_HPP
