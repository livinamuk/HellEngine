#include "hellpch.h"
#include "BoundingBox.h"
#include "HellEngine/Util.h"

namespace HellEngine {

	BoundingBox::BoundingBox(glm::vec3 position, glm::vec3 size, ObjectOrigin objectOrigin, std::string name, bool testCollisions)
	{
		this->position = position;
		this->objectOrigin = objectOrigin;
		//this->angle = 0;
		this->width = size.x;
		this->height = size.y;
		this->depth = size.z;
		this->name = name;
		this->TestCollisions = testCollisions;
		//CalculateCorners();
		SetupPlanes();
	}

	BoundingBox::BoundingBox()
	{
	}

	BoundingBox::BoundingBox(glm::vec3 lowestPositionValues, glm::vec3 highestPositionValues)
	{
		glm::vec3 low = lowestPositionValues;
		glm::vec3 high = highestPositionValues;
		/*leftBottomFront = glm::vec3(low.x, low.y, high.z);
		rightBottomFront = glm::vec3(high.x, low.y, high.z);
		leftTopFront = glm::vec3(low.x, high.y, high.z);
		rightTopFront = glm::vec3(high.x, high.y, high.z);
		leftBottomBack = glm::vec3(low.x, low.y, low.z);
		rightBottomBack = glm::vec3(high.x, low.y, low.z);
		leftTopBack = glm::vec3(low.x, high.y, low.z);
		rightTopBack = glm::vec3(high.x, high.y, low.z);*/

		width = high.x - low.x;
		height = high.y - low.y;
		depth = high.z - low.z;

		baseTransform.position = glm::vec3(low.x + (width * 0.5f), low.y + (height * 0.5f), low.z + (depth * 0.5f));
		baseTransform.scale = glm::vec3(width, height, depth);

		leftBottomFront = glm::vec3(-0.5f, -0.5f, 0.5f);
		rightBottomFront = glm::vec3(0.5f, -0.5f, 0.5f);
		leftTopFront = glm::vec3(-0.5f, 0.5f, 0.5f);
		rightTopFront = glm::vec3(0.5f, 0.5f, 0.5f);
		leftBottomBack = glm::vec3(-0.5f, -0.5f, -0.5f);
		rightBottomBack = glm::vec3(0.5f, -0.5f, -0.5f);
		leftTopBack = glm::vec3(-0.5f, 0.5f, -0.5f);
		rightTopBack = glm::vec3(0.5f, 0.5f, -0.5f);

		SetupPlanes();
		//std::cout << "YOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n";

		//position = glm::vec3(0);

		//std::cout << "Lowest:  " + Util::Vec3ToString(lowestPositionValues) << "\n";
		//std::cout << "Highest: " + Util::Vec3ToString(highestPositionValues) << "\n";
	}

	BoundingBox::~BoundingBox()
	{
	}

	/*void BoundingBox::CalculateCorners()
	{
		return;
		///////////////////////////////////////////
		if (objectOrigin == BOTTOM_CENTERED)
		{
			leftBottomFront = glm::vec3(-width / 2, 0, depth / 2);
			rightBottomFront = glm::vec3(width / 2, 0, depth / 2);
			leftTopFront = glm::vec3(-width / 2, height, depth / 2);
			rightTopFront = glm::vec3(width / 2, height, depth / 2);
			leftBottomBack = glm::vec3(-width / 2, 0, -depth / 2);
			rightBottomBack = glm::vec3(width / 2, 0, -depth / 2);
			leftTopBack = glm::vec3(-width / 2, height, -depth / 2);
			rightTopBack = glm::vec3(width / 2, height, -depth / 2);
		}
		else if (objectOrigin == BOTTOM_LEFT_CENTERED)
		{
			leftBottomFront = glm::vec3(0, 0, depth / 2);
			rightBottomFront = glm::vec3(width, 0, depth / 2);
			leftTopFront = glm::vec3(0, height, depth / 2);
			rightTopFront = glm::vec3(width, height, depth / 2);
			leftBottomBack = glm::vec3(0, 0, -depth / 2);
			rightBottomBack = glm::vec3(width, 0, -depth / 2);
			leftTopBack = glm::vec3(0, height, -depth / 2);
			rightTopBack = glm::vec3(width, height, -depth / 2);
		}
		else if (objectOrigin == BOTTOM_LEFT_FRONT_CORNER)
		{
			leftBottomFront = glm::vec3(0, 0, 0);
			rightBottomFront = glm::vec3(width, 0, 0);
			leftTopFront = glm::vec3(0, height, 0);
			rightTopFront = glm::vec3(width, height, 0);
			leftBottomBack = glm::vec3(0, 0, -depth);
			rightBottomBack = glm::vec3(width, 0, -depth);
			leftTopBack = glm::vec3(0, height, -depth);
			rightTopBack = glm::vec3(width, height, -depth);
		}
		else if (objectOrigin == BOTTOM_LEFT_BACK_CORNER)
		{
			leftBottomFront = glm::vec3(0, 0, depth);
			rightBottomFront = glm::vec3(width, 0, depth);
			leftTopFront = glm::vec3(0, height, depth);
			rightTopFront = glm::vec3(width, height, depth);
			leftBottomBack = glm::vec3(0, 0, 0);
			rightBottomBack = glm::vec3(width, 0, 0);
			leftTopBack = glm::vec3(0, height, 0);
			rightTopBack = glm::vec3(width, height, 0);
		}
		else if (objectOrigin == CENTERED)
		{
			leftBottomFront = glm::vec3(-width / 2, -height / 2, depth / 2);
			rightBottomFront = glm::vec3(width/2, -height / 2, depth / 2);
			leftTopFront = glm::vec3(-width / 2, height/2, depth / 2);
			rightTopFront = glm::vec3(width/2, height/2, depth / 2);
			leftBottomBack = glm::vec3(-width / 2, -height / 2, -depth / 2);
			rightBottomBack = glm::vec3(width/2, - height / 2, -depth / 2);
			leftTopBack = glm::vec3(-width / 2, height/2, -depth / 2);
			rightTopBack = glm::vec3(width/2, height/2, -depth / 2);
		}

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, position);
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		leftBottomFront = glm::vec3(model * glm::vec4(leftBottomFront, 1));
		rightBottomFront = glm::vec3(model * glm::vec4(rightBottomFront, 1));
		leftTopFront = glm::vec3(model * glm::vec4(leftTopFront, 1));
		rightTopFront = glm::vec3(model * glm::vec4(rightTopFront, 1));
		leftBottomBack = glm::vec3(model * glm::vec4(leftBottomBack, 1));
		rightBottomBack = glm::vec3(model * glm::vec4(rightBottomBack, 1));
		leftTopBack = glm::vec3(model * glm::vec4(leftTopBack, 1));
		rightTopBack = glm::vec3(model * glm::vec4(rightTopBack, 1));
	}*/

	void BoundingBox::Draw(Shader *shader)
	{
		frontPlane.Draw(shader);
		backPlane.Draw(shader);
		leftPlane.Draw(shader);
		rightPlane.Draw(shader);
	}

	void BoundingBox::SetupPlanes()
	{
		frontPlane = BoundingPlane(leftTopFront, rightTopFront, rightBottomFront, leftBottomFront, TestCollisions, name);
	//	frontPlane = BoundingPlane(leftBottomFront, rightBottomFront, rightTopFront, leftTopFront);
	
		backPlane = BoundingPlane(rightTopBack, leftTopBack, leftBottomBack, rightBottomBack, TestCollisions, name);
		leftPlane = BoundingPlane(leftTopBack, leftTopFront, leftBottomFront, leftBottomBack, TestCollisions, name);
		rightPlane = BoundingPlane(rightTopFront, rightTopBack, rightBottomBack, rightBottomFront, TestCollisions, name);

		topPlane = BoundingPlane(leftTopBack, rightTopBack, rightTopFront, leftTopFront, false, name);
	}

	/*void BoundingBox::SetAngle(float angle)
	{
		this->angle = angle; 
		CalculateCorners();
		SetupPlanes();
	}*/
}