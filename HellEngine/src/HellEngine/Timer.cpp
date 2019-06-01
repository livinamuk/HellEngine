#include "hellpch.h"
#include "Timer.h"

namespace HellEngine
{
	Timer::Timer(std::string timerName)
	{
		name = timerName;
		startTime = std::chrono::high_resolution_clock::now();
	}


	Timer::~Timer()
	{
		auto endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = endTime - startTime;
		std::cout << name << " took " << (duration.count() * 1000) << " seconds \n";
	}
}
