#pragma once

#include <chrono>
#include <thread>

namespace HellEngine
{
	class Timer
	{
	public:
		Timer(std::string timerName);
		~Timer();

		std::string name;
		std::chrono::steady_clock::time_point startTime;
	};
}