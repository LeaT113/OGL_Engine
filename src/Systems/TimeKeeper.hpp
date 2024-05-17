#ifndef OGL_ENGINE_TIMEKEEPER_HPP
#define OGL_ENGINE_TIMEKEEPER_HPP

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
	double _lastUpdateTime = 0;
	double _timeSinceStartup = 0;
	double _deltaTime = 0;
	bool _logging = false;
};


#endif
