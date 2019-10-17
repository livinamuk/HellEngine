#include "hellpch.h"
#include "Wall.h"
#include "../../Platform/OpenGL/mesh.h"

namespace HellEngine {

	Model* Wall::model = nullptr;
	//const float WALL_WIDTH = 1;
	const float WALL_HEIGHT = 2.4f;
	//unsigned int modelMatricesVBO = 0;

	Wall::Wall(float x, float z, Axis axis, float width, Material* material)
	{
		position = glm::vec3(x, 0, z);
		this->width = width;
		this->axis = axis;
		this->scale = glm::vec3(width, 1, 1);
		this->material = material;

		if (axis == Axis::X) {
			angle = ROTATE_0;
			normal = glm::vec3(0, 0, 1);
		}
		else if (axis == Axis::X_NEGATIVE) {
			angle = ROTATE_180;
			normal = glm::vec3(0, 0, -1);
		}
		else if (axis == Axis::Z) {
			angle = ROTATE_90;
			normal = glm::vec3(1, 0, 0);
		}
		else if (axis == Axis::Z_NEGATIVE) {
			angle = ROTATE_270;
			normal = glm::vec3(-1, 0, 0);
		}

		SetupPlane();
	}

	Wall::~Wall()
	{
		// Destuctor
	}

	void Wall::Draw(Shader* shader, bool bindTextures)
	{
		if (bindTextures) {
			material->BindTextures();
		}
		shader->setMat4("model", modelMatrix);
		this->model->Draw(shader);
	}

	glm::mat4 Wall::GetModelMatrix()
	{
		return modelMatrix;
	}

	void Wall::CalculateModelMatrix()
	{
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, scale);
	}

	void Wall::SetupPlane()
	{
		glm::vec3 topLeft = glm::vec3(0, WALL_HEIGHT, 0);
		glm::vec3 topRight = glm::vec3(width, WALL_HEIGHT, 0);
		glm::vec3 bottomLeft = glm::vec3(0, 0, 0);
		glm::vec3 bottomRight = glm::vec3(width, 0, 0);

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, position);
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		topLeft = glm::vec3(model * glm::vec4(topLeft, 1));
		topRight = glm::vec3(model * glm::vec4(topRight, 1));
		bottomLeft = glm::vec3(model * glm::vec4(bottomLeft, 1));
		bottomRight = glm::vec3(model * glm::vec4(bottomRight, 1));

		boundingPlane = BoundingPlane(topLeft, topRight, bottomRight, bottomLeft, true, "Wall");
	}
}