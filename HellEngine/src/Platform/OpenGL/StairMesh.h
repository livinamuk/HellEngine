#pragma once
#pragma once
#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Transform.h"
#include "HellEngine/Util.h"

// I'm assumign that the 4 verts here make a plane. Coz otherwise the Tangent/BiTangent would be different for 2nd tri.

namespace HellEngine
{
	class StairMesh
	{
	public: // methods
		StairMesh();
		StairMesh(int stepCount);
		~StairMesh();
		void Draw(Shader* shader);
		void ClearMesh();
		void AddQuad(glm::vec3 cornerBottomLeft, glm::vec3 cornerTopRight, Axis axis);
		void AddTopQuad(glm::vec3 cornerBottomLeft, glm::vec3 cornerTopRight);

	public: // fields
		unsigned int VAO = 0;
		unsigned int VBO, EBO;
		Transform transform;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		std::vector<BoundingPlane> boundingPlanes;

		float stepWidth = 1.2f;
		float stepHeight = 0.15f;
		float stepDepth = 0.15f;
		
		float startX = 0;
		float startY = 0;
		float startZ = 8;

		float stepCount;

	private: // methods
		void BufferMeshToGL();
	};
}