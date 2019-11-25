#pragma once
#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Transform.h"
#include "HellEngine/Util.h"

// I'm assumign that the 4 verts here make a plane. Coz otherwise the Tangent/BiTangent would be different for 2nd tri.

namespace HellEngine
{
	class WallMesh
	{
	public: // methods
		WallMesh();
		~WallMesh();
		void Draw(Shader* shader);
		void ClearMesh();
		void AddQuad(glm::vec3 cornerBottomLeft, glm::vec3 cornerTopRight, Axis axis);

	public: // fields
		unsigned int VAO = 0;
		unsigned int VBO, EBO;
		Transform transform;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

	private: // methods
		void BufferMeshToGL();
	};
}