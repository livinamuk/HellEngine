#include "hellpch.h"
#include "Util.h"
#include "imgui.h"
#include <iomanip> // setprecision
#include <sstream> // stringstream

namespace HellEngine
{

	std::string Util::OUTPUT_TEXT;
	glm::mat4 Util::weaponModelMatrix;
	
	//template void Util::OUTPUT(const char*, float);
	//template void Util::OUTPUT(const char*, int);		
	
	float Util:: RandomFloat(float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}

	void Util::SetNormalsAndTangentsFromVertices(Vertex* vert0, Vertex* vert1, Vertex* vert2)
	{
		// Shortcuts for UVs
		glm::vec3& v0 = vert0->Position;
		glm::vec3& v1 = vert1->Position;
		glm::vec3& v2 = vert2->Position;
		glm::vec2& uv0 = vert0->TexCoords;
		glm::vec2& uv1 = vert1->TexCoords;
		glm::vec2& uv2 = vert2->TexCoords;

		// Edges of the triangle : postion delta. UV delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;		
		
		glm::vec3 normal = Util::NormalFromTriangle(vert0->Position, vert1->Position, vert2->Position);

		vert0->Normal = normal;
		vert1->Normal = normal;
		vert2->Normal = normal;

		vert0->Tangent = tangent;
		vert1->Tangent = tangent;
		vert2->Tangent = tangent;

		vert0->Bitangent = bitangent;
		vert1->Bitangent = bitangent;
		vert2->Bitangent = bitangent;
	}

	glm::mat4 Util::Normal_To_Rotation_Matrix(glm::vec3 normal)
	{
			// Find a vector in the plane
			glm::vec3 tangent0 = glm::cross(normal, glm::vec3(1, 0, 0));
			if (glm::dot(tangent0, tangent0) < 0.001f)
				tangent0 = glm::cross(normal, glm::vec3(0, 1, 0));
			tangent0 = glm::normalize(tangent0);
			// Find another vector in the plane
			glm::vec3 tangent1 = glm::normalize(glm::cross(normal, tangent0));
			// Construct a 3x3 matrix by storing three vectors in the columns of the matrix
			
			glm::mat4 mat = glm::mat4(1);
			mat[0][0] = tangent0.x;
			mat[1][0] = tangent0.y;
			mat[2][0] = tangent0.z;
			mat[0][1] = tangent1.x;
			mat[1][1] = tangent1.y;
			mat[2][1] = tangent1.z;
			mat[0][2] = normal.x;
			mat[1][2] = normal.y;
			mat[2][2] = normal.z;
			return mat;// return ColumnVectorsToMatrix(tangent0, tangent1, normal);
	}

	glm::vec3 Util::Vec3InterpTo(glm::vec3 Current, glm::vec3 Target, float DeltaTime, float InterpSpeed)
	{
		float x = FInterpTo(Current.x, Target.x, DeltaTime, InterpSpeed);
		float y = FInterpTo(Current.y, Target.y, DeltaTime, InterpSpeed);
		float z = FInterpTo(Current.z, Target.z, DeltaTime, InterpSpeed);
		return glm::vec3(x, y, z);
	}

	float Util::FInterpTo(float Current, float Target, float DeltaTime, float InterpSpeed)
	{
		// If no interp speed, jump to target value
		if (InterpSpeed <= 0.f)
			return Target;

		// Distance to reach
		const float Dist = Target - Current;

		// If distance is too small, just set the desired location
		if (Util::Square(Dist) < SMALL_NUMBER)
			return Target;

		// Delta Move, Clamp so we do not over shoot.
		const float DeltaMove = Dist * std::clamp(DeltaTime * InterpSpeed, 0.0f, 1.0f);
		return Current + DeltaMove;
	}

	glm::mat4 Util::EmptyMat4()
	{
		return glm::mat4(0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0);
	}

	std::string Util::Vec3ToString(glm::vec3 v)
	{
		return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
	}

	const char* Util::Vec3ToCString(glm::vec3 v)
	{
		return Vec3ToString(v).c_str();
	}

	float Util::Square(float value)
	{
		return value * value;
	}

	void Util::printMat4(glm::mat4 m)
	{
		std::cout << "(" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << ")\n";
		std::cout << "(" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << ")\n";
		std::cout << "(" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << ")\n";
		std::cout << "(" << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << ")\n";
	}

	std::string Util::Mat4ToString(glm::mat4 m)
	{
		std::string s = "";
		s += "(" + std::to_string(m[0][0]) + ", " + std::to_string(m[0][1]) + ", " + std::to_string(m[0][2]) + ", " + std::to_string(m[0][3]) + ")\n";
		s += "(" + std::to_string(m[1][0]) + ", " + std::to_string(m[1][1]) + ", " + std::to_string(m[1][2]) + ", " + std::to_string(m[1][3]) + ")\n";
		s += "(" + std::to_string(m[2][0]) + ", " + std::to_string(m[2][1]) + ", " + std::to_string(m[2][2]) + ", " + std::to_string(m[2][3]) + ")\n";
		s += "(" + std::to_string(m[3][0]) + ", " + std::to_string(m[3][1]) + ", " + std::to_string(m[3][2]) + ", " + std::to_string(m[3][3]) + ")\n";
		return s;
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

	bool Util::StringEndsIn(std::string const &fullString, std::string const &ending) {
		if (fullString.length() >= ending.length()) {
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		else {
			return false;
		}
	}

	std::string Util::RemoveCharactersFromEnd(std::string string, int number)
	{
		std::string result = string.substr(0, string.length() - number);
		return result;
	}

	std::string Util::RemoveCharactersFromBeginning(std::string string, int number)
	{
		std::string result = string.substr(number, string.length() - number);
		return result; 
	}

	/*bool stringEndsIn(std::string input, std::string query)
	{
		int position = input.size() - query.size();

		if (position < 0)
			return false;
		
		if (input.substr(position, query.size()) == query)
			return true;
		else 
			return false;
	}*/

	std::string Util::FloatToString(float value, int precision)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(precision) << value;
		return stream.str();
	}

	std::string Util::AxisToString(Axis axis)
	{
		if (axis == Axis::X)
			return "X";
		if (axis == Axis::Z)
			return "Z";
		if (axis == Axis::X_NEGATIVE)
			return "X_NEGATIVE";
		if (axis == Axis::Z_NEGATIVE)
			return "Z_NEGATIVE";
		else
			return "INVALID AXIS";
	}

	Axis Util::StringToAxis(std::string string)
	{
		if (string == "X")
			return Axis::X;
		if (string == "Z")
			return Axis::Z;
		if (string == "X_NEGATIVE")
			return Axis::X_NEGATIVE;
		else
			return Axis::Z_NEGATIVE;
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

	glm::mat4 Util::Mat4InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
	{
		glm::mat4 m = glm::mat4(1);
		m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
		m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
		m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
		m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
		return m;
	}

	glm::mat4 Util::Mat4InitRotateTransform(float RotateX, float RotateY, float RotateZ)
	{
		glm::mat4 rx = glm::mat4(1);
		glm::mat4 ry = glm::mat4(1);
		glm::mat4 rz = glm::mat4(1);

		const float x = ToRadian(RotateX);
		const float y = ToRadian(RotateY);
		const float z = ToRadian(RotateZ);

		rx[0][0] = 1.0f; rx[0][1] = 0.0f; rx[0][2] = 0.0f; rx[0][3] = 0.0f;
		rx[1][0] = 0.0f; rx[1][1] = cosf(x); rx[1][2] = -sinf(x); rx[1][3] = 0.0f;
		rx[2][0] = 0.0f; rx[2][1] = sinf(x); rx[2][2] = cosf(x); rx[2][3] = 0.0f;
		rx[3][0] = 0.0f; rx[3][1] = 0.0f; rx[3][2] = 0.0f; rx[3][3] = 1.0f;

		ry[0][0] = cosf(y); ry[0][1] = 0.0f; ry[0][2] = -sinf(y); ry[0][3] = 0.0f;
		ry[1][0] = 0.0f; ry[1][1] = 1.0f; ry[1][2] = 0.0f; ry[1][3] = 0.0f;
		ry[2][0] = sinf(y); ry[2][1] = 0.0f; ry[2][2] = cosf(y); ry[2][3] = 0.0f;
		ry[3][0] = 0.0f; ry[3][1] = 0.0f; ry[3][2] = 0.0f; ry[3][3] = 1.0f;

		rz[0][0] = cosf(z); rz[0][1] = -sinf(z); rz[0][2] = 0.0f; rz[0][3] = 0.0f;
		rz[1][0] = sinf(z); rz[1][1] = cosf(z); rz[1][2] = 0.0f; rz[1][3] = 0.0f;
		rz[2][0] = 0.0f; rz[2][1] = 0.0f; rz[2][2] = 1.0f; rz[2][3] = 0.0f;
		rz[3][0] = 0.0f; rz[3][1] = 0.0f; rz[3][2] = 0.0f; rz[3][3] = 1.0f;

		return rz * ry * rx;
	}

	glm::mat4 Util::Mat4InitTranslationTransform(float x, float y, float z)
	{
		glm::mat4 m = glm::mat4(1);
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
		return m;
	}

	glm::mat4 Util::aiMatrix4x4ToGlm(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	glm::mat4 Util::aiMatrix3x3ToGlm(const aiMatrix3x3& from)
	{
		glm::mat4 m;
		m[0][0] = from.a1; m[0][1] = from.a2; m[0][2] = from.a3; m[0][3] = 0.0f;
		m[1][0] = from.b1; m[1][1] = from.b2; m[1][2] = from.b3; m[1][3] = 0.0f;
		m[2][0] = from.c1; m[2][1] = from.c2; m[2][2] = from.c3; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
		return m;
	}

	//template <class T>
	void Util::OUTPUT(const char* text, glm::vec3 value)
	{
		OUTPUT_TEXT += text;
		OUTPUT_TEXT += " (";
		char buffer[128];

		snprintf(buffer, sizeof buffer, "%f", value.x);
		OUTPUT_TEXT += buffer;
		OUTPUT_TEXT += ", ";

		snprintf(buffer, sizeof buffer, "%f", value.y);
		OUTPUT_TEXT += buffer;
		OUTPUT_TEXT += ", ";

		snprintf(buffer, sizeof buffer, "%f", value.z);
		OUTPUT_TEXT += buffer;
		OUTPUT_TEXT += ")\n";
	}

	void Util::OUTPUT(const char* text, float value)
	{
		char buffer[128];
		snprintf(buffer, sizeof buffer, "%f", value);
		OUTPUT_TEXT += text;
		OUTPUT_TEXT += buffer;
		OUTPUT_TEXT += "\n";
	}

	void Util::OUTPUT(const char* text, const char* value)
	{
		OUTPUT_TEXT += text;
		OUTPUT_TEXT += value;
		OUTPUT_TEXT += "\n";
	}

	void Util::OUTPUT(const char* text, int value)
	{
		char buffer[128];
		snprintf(buffer, sizeof buffer, "%f", value);
	//	char* ptr = reinterpret_cast<char*>(&value);
		OUTPUT_TEXT += text;
		OUTPUT_TEXT += buffer;
		OUTPUT_TEXT += "\n";
	}

	bool Util::StringBeginsWith(std::string mainString, std::string subString)
	{
		if (mainString.find(subString) == 0)
			return true;
		else return false;
	}

	/*IntersectionData Util::RayIntersectPlane(glm::vec3 plane_origin, glm::vec3 plane_normal, glm::vec3 ray_origin, glm::vec3 ray_direction)
	{
		IntersectionData result;
		result.distance = glm::dot(plane_normal, (plane_origin - ray_origin)) / glm::dot(plane_normal, ray_direction);
		result.location = ray_origin + (ray_direction * result.distance);
		return result;
	}

	CollisionData Util::RayIntersectFloor(Floor floor, glm::vec3 ray_origin, glm::vec3 ray_direction)
	{
		CollisionData result;
		result.occured = false;
		glm::vec3 plane_normal(0, 1, 0);
		result.intersectionData.distance = glm::dot(plane_normal, (floor.position - ray_origin)) / glm::dot(plane_normal, ray_direction);
		result.intersectionData.location = ray_origin + (ray_direction * result.intersectionData.distance);

		if (result.intersectionData.location.x < floor.position.x)
			return result;
		if (result.intersectionData.location.x > floor.position.x + floor.width)
			return result;
		if (result.intersectionData.location.z < floor.position.z)
			return result;
		if (result.intersectionData.location.z > floor.position.z + floor.depth)
			return result;

		// Guess you're on it then
		result.occured = true;
		return result;
	}
	CollisionData Util::RayIntersectCeiling(Ceiling ceiling, glm::vec3 ray_origin, glm::vec3 ray_direction)
	{
		CollisionData result;
		result.occured = false;
		glm::vec3 plane_normal(0, -1, 0);
		glm::vec3 plane_origin(ceiling.position.x, ceiling.position.y + 2.4f, ceiling.position.z);
		result.intersectionData.distance = glm::dot(plane_normal, (plane_origin - ray_origin)) / glm::dot(plane_normal, ray_direction);
		result.intersectionData.location = ray_origin + (ray_direction * result.intersectionData.distance);

		if (result.intersectionData.location.x < ceiling.position.x)
			return result;
		if (result.intersectionData.location.x > ceiling.position.x + ceiling.width)
			return result;
		if (result.intersectionData.location.z < ceiling.position.z)
			return result;
		if (result.intersectionData.location.z > ceiling.position.z + ceiling.depth)
			return result;

		// Guess you're on it then
		result.occured = true;
		return result;
	}

	CollisionData Util::RayIntersectsBoundingPlane(glm::vec3 ray_origin, glm::vec3 ray_direction, BoundingPlane* boundingPlane)
	{
		CollisionData result; 
		result.occured = false;
		result.intersectionData = RayIntersectPlane(boundingPlane->A, boundingPlane->normal, ray_origin, ray_direction);
		glm::vec3 intersectionPoint = result.intersectionData.location;

		// Leave early
		if (result.intersectionData.distance > 0)
			return result;
		if (intersectionPoint.y > boundingPlane->A.y)
			return result;
		if (intersectionPoint.y < boundingPlane->C.y)
			return result;

		float distanceAtoB = glm::length(boundingPlane->A - boundingPlane->B);
		float disntanceFromB = glm::length(glm::vec3(intersectionPoint.x, boundingPlane->A.y, intersectionPoint.z) - boundingPlane->A);
		float disntanceFromA = glm::length(glm::vec3(intersectionPoint.x, boundingPlane->B.y, intersectionPoint.z) - boundingPlane->B);

		if (disntanceFromA > distanceAtoB)
			return result;
		if (disntanceFromB > distanceAtoB)
			return result;

		// If ya reach here you are on the plane.
		result.occured = true;
		return result;
	}*/


	std::string Util::Vec3_to_String(glm::vec3 v)
	{
		std::string s = "(";
		s += std::to_string(v.x);
		s += ", ";
		s += std::to_string(v.y);
		s += ", ";
		s += std::to_string(v.z);
		s += ")";
		return s;
	}	
	
	std::string Util::Mat4_to_String(glm::mat4 m)
	{
		std::string s;
		s += "[" + std::to_string(m[0][0]) + ", " + std::to_string(m[1][0]) + ", " + std::to_string(m[2][0]) + ", " + std::to_string(m[3][0]) + "]\n";
		s += "[" + std::to_string(m[0][1]) + ", " + std::to_string(m[1][1]) + ", " + std::to_string(m[2][1]) + ", " + std::to_string(m[3][1]) + "]\n";
		s += "[" + std::to_string(m[0][2]) + ", " + std::to_string(m[1][2]) + ", " + std::to_string(m[2][2]) + ", " + std::to_string(m[3][2]) + "]\n";
		s += "[" + std::to_string(m[0][3]) + ", " + std::to_string(m[1][3]) + ", " + std::to_string(m[2][3]) + ", " + std::to_string(m[3][3]) + "]\n";
		return s;
	}

	CollisionData Util::RayTriangleIntersect(glm::vec3 orig, glm::vec3 dir, glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2)
	{
		CollisionData collision;
		collision.distance = 0;
		collision.occured = false;
		collision.location = glm::vec3(0);

		const float EPSILON = 0.0000001f;
		glm::vec3 edge1, edge2, h, s, q;
		float a, f, u, v;
		edge1 = vertex1 - vertex0;
		edge2 = vertex2 - vertex0;
		//h = rayVector.crossProduct(edge2);
		h = glm::cross(dir, edge2);

		//a = edge1.dotProduct(h);
		a = glm::dot(edge1, h);
		if (a > -EPSILON && a < EPSILON)
			return collision;    // This ray is parallel to this triangle.
		f = 1.0 / a;
		s = orig - vertex0;
		u = f * glm::dot(s, h);
		if (u < 0.0 || u > 1.0)
			return collision;
		q = glm::cross(s, edge1);
		v = f * glm::dot(dir, q);
		if (v < 0.0 || u + v > 1.0)
			return collision;
		// At this stage we can compute t to find out where the intersection point is on the line.
		float t = f * glm::dot(edge2, q);
		if (t > EPSILON) // ray intersection
		{
			collision.location = orig + dir * t;
			collision.occured = true;
			collision.distance = t;
			//std::cout << "FOUND D: " << t << "        ";
			return collision;
		}
		else // This means that there is a line intersection but not a ray intersection.
			return collision;
	}

	glm::vec3 Util::NormalFromTriangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2)
	{
		return glm::normalize(glm::cross(pos1 - pos0, pos2 - pos0));
	}

	/*PlayerPlaneCollisionData Util::PlayerPlaneCollision(glm::vec3 playerPosition, float playerRadius, BoundingPlane* plane)
	{
		PlayerPlaneCollisionData data;
		data.occured = false;
		data.distance = 0;
		data.newPlayerPosition = playerPosition;

		if (Util::CircleIntersectsLine(playerPosition, plane->A, plane->C, playerRadius))
		{
			data.occured = true;

			glm::vec2 planeNormal = glm::vec2(plane->normal.x, plane->normal.z);
			glm::vec2 spherePosition = glm::vec2(playerPosition.x, playerPosition.z);
			glm::vec2 planePosition = glm::vec2(plane->A.x, plane->A.z);
			float sphereRadius = playerRadius;

			float distToPlane = glm::dot(planeNormal, spherePosition - planePosition);

			if (distToPlane < sphereRadius) {
				glm::vec2 contactPoint = spherePosition - planeNormal * distToPlane;
				float penetrationDepth = sphereRadius - distToPlane;
				glm::vec2 penetrationNormal = planeNormal;
				spherePosition += penetrationNormal * penetrationDepth;

				//HELL_ERROR(std::to_string(distToPlane));
			}
			data.newPlayerPosition.x = spherePosition.x;
			data.newPlayerPosition.z = spherePosition.y;
		}
		return data;
	}*/


	btVector3 Util::glmVec3_to_btVec3(glm::vec3 vector)
	{
		return btVector3(vector.x, vector.y, vector.z);
	}
	glm::vec3 Util::btVec3_to_glmVec3(btVector3 vector)
	{
		return glm::vec3(vector.x(), vector.y(), vector.z());
	}
}

/*
glm::vec2 a(boundingPlane->A.x, boundingPlane->A.z);
glm::vec2 b(boundingPlane->B.x, boundingPlane->B.z);
glm::vec2 point(intersectionPoint.x, intersectionPoint.z);

// Cross product
float crossproduct = (point.y - a.y) * (b.x - a.x) - (point.x - a.x) * (b.y - a.y);
float epsilon = 0.001f;
if (abs(crossproduct) > epsilon)
return result;

// Dot product check
float dotProduct = glm::dot(b - a, point - a);
if (dotProduct < 0)
	return result;

// Must be positive & less than the square of the distance between a and b.
float squaredlengthba = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
if (dotProduct > squaredlengthba)
return result;

// If ya reach here you are on the plane.
result.collision = true;
return result;*/


/*

std::cout << boundingPlane->B.x << "," << boundingPlane->B.y << "," << boundingPlane->B.z << " ";
std::cout << boundingPlane->A.x << "," << boundingPlane->A.y << "," << boundingPlane->A.z << " ";

glm::vec3 n = boundingPlane->B - boundingPlane->A;
std::cout << n.x << "," << n.y << "," << n.z << " ";*/