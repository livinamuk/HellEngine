#pragma once

#ifdef HELL_PLATFORM_WINDOWS
	#ifdef HELL_BUILD_DLL
		#define HELL_API __declspec(dllexport)
	#else
		#define HELL_API __declspec(dllimport)
	#endif
#else
	#error HellEngine only supports Windows!
#endif

#ifdef HELL_DEBUG
 #define HELL_ENABLE_ASSERTS
#endif

#ifdef HELL_ENABLE_ASSERTS
	#define HELL_ASSERT(x, ...) { if(!(x)) { HELL_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define HELL_CORE_ASSERT(x, ...) { if(!(x)) { HELL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define HELL_ASSERT(x, ...)
	#define HELL_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define HELL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)