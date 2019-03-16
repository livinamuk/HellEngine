#pragma once

#ifdef HELL_ENGINE_PLATFORM_WINDOWS
	#ifdef HELL_ENGINE_BUILD_DLL
		#define HELL_ENGINE_API __declspec(dllexport)
	#else
		#define HELL_ENGINE_API __declspec(dllexport)
	#endif
#else
	#error HellEngine only supports windows
#endif
