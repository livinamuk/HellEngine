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
		//auto endTime = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<float> duration = endTime - startTime;
		//std::cout << name << " took " << (duration.count() * 1000) << " seconds \n";	
		
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		auto duration = end - start;
		double ms = duration * 0.001;

		std::cout << name << " took " << ms << " seconds \n";
	}
}
