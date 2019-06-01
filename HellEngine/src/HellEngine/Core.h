#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef HELL_PLATFORM_WINDOWS
	#define HELL_API
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

#define PI				(float)3.14159265358979323846
#define ROTATE_0        (float)0
#define ROTATE_180		(float)PI
#define ROTATE_90		(float)PI * 0.5
#define ROTATE_270		(float)PI * 1.5

// Colours
#define HELL_YELLOW	glm::vec3(1, 1, 0)

enum MousePickType { NotFound, Wall, Door};

enum Axis {X, X_NEGATIVE, Z, Z_NEGATIVE};

enum ObjectOrigin {
	BOTTOM_CENTERED,
	BOTTOM_LEFT_CENTERED,
	BOTTOM_LEFT_FRONT_CORNER,
	BOTTOM_LEFT_BACK_CORNER
};

struct MousePickInfo
{
	MousePickType type;
	int indexInVector;
}; 

struct WallHole
{
	glm::vec3 position;
	Axis axis;
};
