#ifndef OGL_ENGINE_TIME_HPP
#define OGL_ENGINE_TIME_HPP
#include <chrono>

class Time
{
public:
	[[nodiscard]] double TimeSinceStartup() const;
	[[nodiscard]] double DeltaTime() const;

	Time();

	void Update();

private:
	using Duration = std::chrono::duration<long long, std::nano>;
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock, Duration>;

	const TimePoint _startupTime;
	TimePoint _lastUpdateTime;
	double _timeSinceStartup = 0;
	double _deltaTime = 0;
};


#endif //OGL_ENGINE_TIME_HPP
