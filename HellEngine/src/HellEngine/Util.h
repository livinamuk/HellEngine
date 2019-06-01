#pragma once

namespace HellEngine 
{
	class Util
	{

		struct CollisionInfo
		{
			bool collision = false;
			float distance = 1000;
		};


	public:

		static glm::mat4 QuarternionToRotationMatrix(glm::quat q);
		static bool CircleIntersectsLine(glm::vec3 circleCenter, glm::vec3 point1, glm::vec3 point2, float circleRadius);
		static void printMat4(glm::mat4 m);
		static float AxisToAngle(Axis axis);
	};
}

