#ifndef OGL_ENGINE_TIMEKEEPER_HPP
#define OGL_ENGINE_TIMEKEEPER_HPP
#include <chrono>

#include "../Core/HybridSingleton.hpp"

class TimeKeeper : public HybridSingleton<TimeKeeper>
{
public:
	TimeKeeper();

	static double TimeSinceStartup();
	static double DeltaTime();

	void EnableLogging(bool enable);

	void Update();

private:
	using Duration = std::chrono::duration<long long, std::nano>;
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock, Duration>;

	const TimePoint _startupTime;
	TimePoint _lastUpdateTime;
	double _timeSinceStartup = 0;
	double _deltaTime = 0;
	bool _logging = false;
};


#endif
