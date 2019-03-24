#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace HellEngine {

	class HELL_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	public:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define HELL_CORE_TRACE(...)    ::HellEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HELL_CORE_INFO(...)     ::HellEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HELL_CORE_WARN(...)     ::HellEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HELL_CORE_ERROR(...)    ::HellEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HELL_CORE_FATAL(...)    ::HellEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define HELL_TRACE(...)	      ::HellEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define HELL_INFO(...)	      ::HellEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define HELL_WARN(...)	      ::HellEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HELL_ERROR(...)	      ::HellEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define HELL_FATAL(...)	      ::HellEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)