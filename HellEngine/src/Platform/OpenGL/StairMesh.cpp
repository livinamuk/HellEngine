#include "hellpch.h"
#include "StairMesh.h"

namespace HellEngine
{
	StairMesh::StairMesh()
	{
	}

	StairMesh::StairMesh(int stepCount)
	{
		this->stepCount = stepCount;
		ClearMesh();

		for (int i = 0; i < stepCount; i++)
		{
			AddQuad(glm::vec3(startX, startY + (stepHeight * i), startZ + (stepDepth * i)), glm::vec3(startX + stepWidth, startY + stepHeight + (stepHeight * i), startZ + (stepDepth * i)), Axis::X);
			AddTopQuad(glm::vec3(startX, startY + stepHeight + (stepHeight * i), startZ + (stepDepth * i)), glm::vec3(startX + stepWidth, startY + stepHeight + (stepHeight * i), startZ + stepDepth + (stepDepth * i)));
		}

		float xLow = startX;
		float xHigh = startX + (stepWidth);
		float yLow = startY;
		float yHigh = startY + (stepHeight * stepCount);
		float zLow = startZ;
		float zHigh = startZ + (stepDepth * stepCount);
		glm::vec3 a(xLow, yHigh, zHigh);
		glm::vec3 b(xHigh, yHigh, zHigh);
		glm::vec3 c(xHigh, yLow, zLow);
		glm::vec3 d(xLow, yLow, zLow);

		// For a ramp
		boundingPlanes.push_back(BoundingPlane(a, b, c, d, true, "Stairs"));
	}

	StairMesh::~StairMesh()
	{
	}

	void StairMesh::ClearMesh()
	{
		vertices.clear();
		indices.clear();
	}

	void StairMesh::AddTopQuad(glm::vec3 pointA, glm::vec3 pointB)
	{
		Vertex vert0, vert1, vert2, vert3;
		vert0.Position = glm::vec3(pointA.x, pointA.y, pointB.z);
		vert1.Position = glm::vec3(pointB.x, pointA.y, pointB.z);
		vert2.Position = glm::vec3(pointB.x, pointA.y, pointA.z);
		vert3.Position = glm::vec3(pointA.x, pointA.y, pointA.z);
		vert0.TexCoords = glm::vec2(0, 0);
		vert1.TexCoords = glm::vec2(1, 0);
		vert2.TexCoords = glm::vec2(1, 1);
		vert3.TexCoords = glm::vec2(0, 1);

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

	void StairMesh::AddQuad(glm::vec3 cornerBottomLeft, glm::vec3 cornerTopRight, Axis axis)
	{
		Vertex vert0, vert1, vert2, vert3;
		vert0.Position = glm::vec3(cornerBottomLeft.x, cornerBottomLeft.y, cornerBottomLeft.z);
		vert1.Position = glm::vec3(cornerBottomLeft.x, cornerTopRight.y, cornerBottomLeft.z);
		vert2.Position = glm::vec3(cornerTopRight.x, cornerTopRight.y, cornerTopRight.z);
		vert3.Position = glm::vec3(cornerTopRight.x, cornerBottomLeft.y, cornerTopRight.z);
		vert0.TexCoords = glm::vec2(0, 0);
		vert1.TexCoords = glm::vec2(1, 0);
		vert2.TexCoords = glm::vec2(1, 1);
		vert3.TexCoords = glm::vec2(0, 1);

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

	void StairMesh::Draw(Shader* shader)
	{
		shader->setMat4("model", transform.to_mat4());

		if (VAO == 0)
			BufferMeshToGL();

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void StairMesh::BufferMeshToGL()
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