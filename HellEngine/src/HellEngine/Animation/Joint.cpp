#include "hellpch.h"
#include "Joint.h"
#include "HellEngine/Util.h"


namespace HellEngine {
	Joint::Joint(int index, std::string name, glm::mat4 inverseBindTransform)
	{
		this->index = index;
		this->name = name;
		this->inverseBindTransform = inverseBindTransform;


		//std::cout << "\n" << name << "\n";
		//Util::printMat4(inverseBindTransform);


		//std::cout << "\nNAME:  " << name << "\n";
		//Util::printMat4(inverseBindTransform);
	}

	Joint::Joint()
	{
	}

	/*void Joint::calcInverseBindTransforms(glm::mat4 parentBindTransform)
	{
		glm::mat4 bindTransform = localBindTransform * parentBindTransform;
		inverseBindTransform = glm::inverse(bindTransform);

		for (Joint & child : this->children)
			child.calcInverseBindTransforms(bindTransform);
	}

	void Joint::printMat4(glm::mat4 m)
	{
		std::cout << "(" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << ")\n";
		std::cout << "(" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << ")\n";
		std::cout << "(" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << ")\n";
		std::cout << "(" << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << ")\n";
	}*/


}