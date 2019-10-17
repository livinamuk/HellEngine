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

#define ROTATE_0        (float)0
#define ROTATE_180		(float)HELL_PI
#define ROTATE_90		(float)HELL_PI * 0.5
#define ROTATE_270		(float)HELL_PI * 1.5

// Math
#define SMALL_NUMBER		(float)9.99999993922529e-9
#define KINDA_SMALL_NUMBER	(float)0.00001
#define HELL_PI				(float)3.14159265358979323846

// ogldev
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define ToRadian(x) (float)(((x) * HELL_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / HELL_PI))



#define GRAVITY -1.8f

// Colours
#define HELL_YELLOW	glm::vec3(1, 1, 0)


	enum MousePickType { NotFound, Wall, Door };

	enum Axis { X, X_NEGATIVE, Z, Z_NEGATIVE };

	enum ObjectOrigin {
		BOTTOM_CENTERED,
		BOTTOM_LEFT_CENTERED,
		BOTTOM_LEFT_FRONT_CORNER,
		BOTTOM_LEFT_BACK_CORNER,
		CENTERED
	};

	enum DecalType {
		BULLET_HOLE
	};


	// UserIndex
/*	enum RigidBodyType {
		NONE,
		STATIC_ENTITY,
		DYNAMIC_ENTITY,
		DOOR,
		FLOOR,
		WALL,
		NOTE
	};
	*/

	struct BulletRaycastData {
		int index = -1;
		float distance = 0;
		glm::vec3 hitPoint;
		glm::vec3 surfaceNormal;
		std::string name;
	};

struct EntityData {
	std::string name = "NONE";
	int vectorIndex = -1;
};


//	struct Data {
//		std::string something;
//	}

	/*struct IntersectionData
	{
		float distance;
		glm::vec3 location;
	};*/

	enum AssetType {
		MODEL,
		TEXTURE,
	};

	struct AssetInfo
	{
		AssetType type;
		std::string filename;
	};

	struct Vertex2D {
		glm::vec3 Position;
		glm::vec2 TexCoords;
	};


	/*struct RaycastData
	{
		glm::vec3 intersectionPoint;
		float distance;
		int planeIndex;
		std::string itemName;

		RaycastData()
		{
			itemName = "None";
		}
	};*/

	struct CollisionData
	{
		bool occured;
		float distance;
		glm::vec3 location;
		//glm::vec3 planeNormal;

		// DEPRICATE BELOW
		//IntersectionData intersectionData;
	};


	struct PlayerPlaneCollisionData
	{
		bool occured = false;
		float distance = 1000;
		glm::vec3 newPlayerPosition;
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

	struct DoorHole
	{
		glm::vec3 bottomLeft;
		glm::vec3 bottomRight;
		glm::vec3 topLeft;
		glm::vec3 topRight;
	};

	// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
	enum Movement_Direction {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};