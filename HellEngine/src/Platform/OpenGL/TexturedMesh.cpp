#include "hellpch.h"
#include "TexturedMesh.h"

namespace HellEngine
{
	struct VertexData
	{
		glm::vec3 vertex;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	TexturedMesh::TexturedMesh()
	{
	}


	TexturedMesh::~TexturedMesh()
	{
	}

	void TexturedMesh::BufferData()
	{
		float x1 = 4;
		float x2 = 5;
		float y = 0;
		float z1 = 3;
		float z2 = 6;
		glm::vec2 texScale = glm::vec2(1, 1);

		std::vector<VertexData> vertexData;


		//vertexData.push_back()
		/*v  0.0000 2.4000 0.0500
			v  0.0000 2.3896 0.0500
			v  1.0000 2.3896 0.0500
			v  1.0000 2.4000 0.0500
			v  0.0000 2.3743 0.0296
			v  1.0000 2.3743 0.0296
			v  0.0000 2.3474 0.0237
			v  1.0000 2.3474 0.0237
			v  0.0000 2.3277 0.0095
			v  1.0000 2.3277 0.0095
			v  0.0000 2.3183 0.0094
			v  1.0000 2.3183 0.0094
			v  0.0000 2.3183 0.0000
			v  1.0000 2.3183 0.0000
			v  0.0000 0.0913 - 0.0000
			v  1.0000 0.0913 - 0.0000
			v  0.0000 0.0777 0.0100
			v  1.0000 0.0777 0.0100
			v  0.0000 0.0000 0.0100
			v  1.0000 0.0000 0.0100*/

		//VertexData v = { glm::vec3(0.0000 2.4000 0.0500), glm::vec3(0, 1, 0), glm::vec2(0,  1) };

		vertexData.push_back({ glm::vec3(x2,  y,  z2), glm::vec3(0, 1, 0), glm::vec2(0,  1) });
		vertexData.push_back({ glm::vec3(x2,  y,  z1), glm::vec3(0, 1, 0), glm::vec2(1,  1) });
		vertexData.push_back({ glm::vec3(x1,  y,  z1), glm::vec3(0, 1, 0), glm::vec2(1,  0) });
		vertexData.push_back({ glm::vec3(x1,  y,  z1), glm::vec3(0, 1, 0), glm::vec2(1,  0) });
		vertexData.push_back({ glm::vec3(x1,  y,  z2), glm::vec3(0, 1, 0), glm::vec2(0,  0) });
		vertexData.push_back({ glm::vec3(x2,  y,  z2), glm::vec3(0, 1, 0), glm::vec2(0,  1) });



		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData) * 8, &vertexData[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	void TexturedMesh::Draw(Shader* shader, bool bindTextures)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		//modelMatrix = glm::translate(modelMatrix, position);
		//modelMatrix = glm::scale(modelMatrix, scale);
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	}

}