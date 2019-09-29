#pragma once

#include "assimp/matrix4x4.h"
#include "assimp/matrix3x3.h"
#include "Core.h"
#include "Platform/OpenGL/BoundingPlane.h"
#include "HellEngine/Components/Floor.h"
#include "HellEngine/Components/Ceiling.h"

namespace HellEngine 
{
	class Util
	{
	public:
		static glm::mat4 weaponModelMatrix;

		struct CollisionInfo
		{
			bool collision = false;
			float distance = 1000;
		};



	public: // methods
		static float Util::RandomFloat(float a, float b);
		static glm::mat4 QuarternionToRotationMatrix(glm::quat q);
		static bool CircleIntersectsLine(glm::vec3 circleCenter, glm::vec3 point1, glm::vec3 point2, float circleRadius);
		static void printMat4(glm::mat4 m);
		static float AxisToAngle(Axis axis);
		static bool StringEndsIn(std::string const &fullString, std::string const &ending);
		static std::string RemoveCharactersFromEnd(std::string string, int number);
		static std::string RemoveCharactersFromBeginning(std::string string, int number);
		static std::string FloatToString(float value, int precision);
		static std::string AxisToString(Axis axis);
		static Axis StringToAxis(std::string string);
		static float FInterpTo(float Current, float Target, float DeltaTime, float InterpSpeed);
		static glm::vec3 Vec3InterpTo(glm::vec3 Current, glm::vec3 Target, float DeltaTime, float InterpSpeed);
		static float Square(float value);
		static std::string Vec3ToString(glm::vec3 vector);
		static const char* Vec3ToCString(glm::vec3 vector);
		static glm::mat4 EmptyMat4();
		static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
		static glm::mat4 aiMatrix3x3ToGlm(const aiMatrix3x3& from);
		static glm::mat4 Mat4InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
		static glm::mat4 Mat4InitRotateTransform(float RotateX, float RotateY, float RotateZ);
		static glm::mat4 Mat4InitTranslationTransform(float x, float y, float z);
		static std::string Mat4ToString(glm::mat4 m);
		static bool StringBeginsWith(std::string mainString, std::string subString);
		
		static std::string OUTPUT_TEXT;
		static void OUTPUT(std::string text);

		static std::string Vec3_to_String(glm::vec3 vector);
		static std::string Mat4_to_String(glm::mat4 matrix);
		static glm::mat4 Normal_To_Rotation_Matrix(glm::vec3 normal);

		//static IntersectionData RayIntersectPlane(glm::vec3 plane_origin, glm::vec3 plane_normal, glm::vec3 ray_origin, glm::vec3 ray_direction);
		//static CollisionData RayIntersectsBoundingPlane(glm::vec3 ray_origin, glm::vec3 ray_direction, BoundingPlane* boundingPlane);

		static CollisionData Util::RayIntersectFloor(Floor floor, glm::vec3 ray_origin, glm::vec3 ray_direction);
		static CollisionData Util::RayIntersectCeiling(Ceiling ceiling, glm::vec3 ray_origin, glm::vec3 ray_direction);

		static CollisionData RayTriangleIntersect(glm::vec3 orig, glm::vec3 dir, glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2);

		//static void ImguiFloat3(std::string text, glm::vec3* vector);


	public: // fields
		//static float SMALL_NUMBER = 9.99999993922529e-9;
	private:
	};
	struct VertexBoneData
	{
		unsigned int IDs[4];
		float Weights[4];

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		void AddBoneData(unsigned int BoneID, float Weight);
	};
}

