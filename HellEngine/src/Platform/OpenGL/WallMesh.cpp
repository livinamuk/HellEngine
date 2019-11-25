#include "hellpch.h"
#include "WallMesh.h"

namespace HellEngine
{
	WallMesh::WallMesh()
	{
	}

	WallMesh::~WallMesh()
	{
	}

	void WallMesh::ClearMesh()
	{
		vertices.clear();
		indices.clear();
	}
	
	void WallMesh::AddQuad(glm::vec3 cornerBottomLeft, glm::vec3 cornerTopRight, Axis axis)
	{
		Vertex vert0, vert1, vert2, vert3;
		vert0.Position = glm::vec3(cornerTopRight.x, cornerBottomLeft.y, cornerTopRight.z);
		vert1.Position = glm::vec3(cornerTopRight.x, cornerTopRight.y, cornerTopRight.z);
		vert2.Position = glm::vec3(cornerBottomLeft.x, cornerTopRight.y, cornerBottomLeft.z);
		vert3.Position = glm::vec3(cornerBottomLeft.x, cornerBottomLeft.y, cornerBottomLeft.z);


		// Tex coord shit
		float horizontalTexCoordBegin;
		float horizontalTexCoordWidth = (cornerTopRight.x - cornerBottomLeft.x) / 2.4f;

		if (axis == X) {
			horizontalTexCoordBegin = cornerBottomLeft.x / 2.4f;
			horizontalTexCoordWidth = (cornerTopRight.x - cornerBottomLeft.x) / 2.4f;
		}
		if (axis == X_NEGATIVE) {
			horizontalTexCoordBegin = -cornerBottomLeft.x / 2.4f;
			horizontalTexCoordWidth = -(cornerTopRight.x - cornerBottomLeft.x) / 2.4f;

		}
		if (axis == Z) {
			horizontalTexCoordBegin = -cornerBottomLeft.z / 2.4f;
			horizontalTexCoordWidth = -(cornerTopRight.z - cornerBottomLeft.z) / 2.4f;
		}

		if (axis == Z_NEGATIVE) {
			horizontalTexCoordBegin = cornerBottomLeft.z / 2.4f;
			horizontalTexCoordWidth = (cornerTopRight.z - cornerBottomLeft.z) / 2.4f;
		}

		float topTexCoord = 1 - (cornerTopRight.y / 2.4f);
		float botTexCoord = 1 - (cornerBottomLeft.y / 2.4f);// (cornerTopRight.y + cornerBottomLeft.y) / 2.4f; // 0;// cornerBottomLeft.y / 2.4f;

		vert0.TexCoords = glm::vec2(horizontalTexCoordBegin + horizontalTexCoordWidth, botTexCoord);
		vert1.TexCoords = glm::vec2(horizontalTexCoordBegin + horizontalTexCoordWidth, topTexCoord);
		vert2.TexCoords = glm::vec2(horizontalTexCoordBegin, topTexCoord);
		vert3.TexCoords = glm::vec2(horizontalTexCoordBegin, botTexCoord);

		Util::SetNormalsAndTangentsFromVertices(&vert0, &vert1, &vert2);
		Util::SetNormalsAndTangentsFromVertices(&vert3, &vert0, &vert1);

		unsigned int i = vertices.size();

		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 2);

		indices.push_back(i + 2);
		indices.push_back(i + 3);
		indices.push_back(i);

		vertices.push_back(vert0);
		vertices.push_back(vert1);
		vertices.push_back(vert2);
		vertices.push_back(vert3);
	}

	void WallMesh::Draw(Shader* shader)
	{
		shader->setMat4("model", transform.to_mat4());
		
		if (VAO == 0)
			BufferMeshToGL();

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); 
	}

	void WallMesh::BufferMeshToGL()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}