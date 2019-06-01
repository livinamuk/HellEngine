#include "hellpch.h"
#include "Util.h"

namespace HellEngine
{
	void Util::printMat4(glm::mat4 m)
	{
		std::cout << "(" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << ")\n";
		std::cout << "(" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << ")\n";
		std::cout << "(" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << ")\n";
		std::cout << "(" << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << ")\n";
	}

	float Util::AxisToAngle(Axis axis)
	{
		if (axis == X)
			return 0;
		else if (axis == X_NEGATIVE)
			return ROTATE_180;
		else if (axis == Z)
			return ROTATE_90;
		else if (axis == Z_NEGATIVE)
			return ROTATE_270;
	}

	bool Util::CircleIntersectsLine(glm::vec3 circleCenter, glm::vec3 point1, glm::vec3 point2, float circleRadius)
	{
		// Return early too far away to bother checking.
		float minX = std::min(point1.x, point2.x);
		float minZ = std::min(point1.z, point2.z);
		float maxX = std::max(point1.x, point2.x);
		float maxZ = std::max(point1.z, point2.z);
		if (circleCenter.x + circleRadius < minX) return false;
		if (circleCenter.x - circleRadius > maxX) return false;
		if (circleCenter.z + circleRadius < minZ) return false;
		if (circleCenter.z - circleRadius > maxZ) return false;

		// Begin circle line intersection test...
		glm::vec2 circ_pos = glm::vec2(circleCenter.x, circleCenter.z);
		glm::vec2 seg_a = glm::vec2(point1.x, point1.z);
		glm::vec2 seg_b = glm::vec2(point2.x, point2.z);
		glm::vec2 closestPoint;

		glm::vec2 seg_v = seg_b - seg_a;
		glm::vec2 pt_v = circ_pos - seg_a;
		glm::vec2 seg_v_unit = seg_v / glm::length(seg_v);

		float proj = glm::dot(pt_v, seg_v_unit);

		if (proj <= 0)
			closestPoint = seg_a;
		else if (proj >= glm::length(seg_v))
			closestPoint = seg_b;
		else {
			glm::vec2 proj_v = seg_v_unit * proj;
			closestPoint = proj_v + seg_a;
		}
		glm::vec2 dist_v = circ_pos - closestPoint;
		//float distance = glm::length(dist_v); YOU MIGHT WANT THIS

		// Not on line
		if (glm::length(dist_v) > circleRadius) {
			return false;
		}
		// Check if closestPoint is on line segment
		float test = glm::dot(closestPoint - seg_b, seg_v) / glm::dot(seg_v, seg_v);

		// It isn't
		if (test > 0 || test < -1) {
			return false;
		}
		// It is
		return true;
	}


	glm::mat4 QuarternionToRotationMatrix(glm::quat q)
	{
		glm::mat4 matrix = glm::mat4(0);
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float xw = q.x * q.w;
		float yz = q.y * q.z;
		float yw = q.y * q.w;
		float zw = q.z * q.w;
		float xSquared = q.x * q.x;
		float ySquared = q.y * q.y;
		float zSquared = q.z * q.z;
		matrix[0][0] = 1 - 2 * (ySquared + zSquared);
		matrix[0][1] = 2 * (xy - zw);
		matrix[0][2] = 2 * (xz + yw);
		matrix[0][4] = 0;
		matrix[1][0] = 2 * (xy + zw);
		matrix[1][1] = 1 - 2 * (xSquared + zSquared);
		matrix[1][2] = 2 * (yz - xw);
		matrix[1][3] = 0;
		matrix[2][0] = 2 * (xz - yw);
		matrix[2][1] = 2 * (yz + xw);
		matrix[2][2] = 1 - 2 * (xSquared + ySquared);
		matrix[2][3] = 0;
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		matrix[3][3] = 1;
		return matrix;
	}
}