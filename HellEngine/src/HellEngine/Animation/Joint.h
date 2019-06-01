#pragma once

namespace HellEngine 
{
	class Joint
	{
	public: // methods
		Joint(int index, std::string name, glm::mat4 inverseBindTransform);
		Joint();
		~Joint() = default;
		//void calcInverseBindTransforms(glm::mat4 parentBindTransform); 
		//void printMat4(glm::mat4 m);

	public: // fields
		int index;
		std::string name;
		std::vector<Joint> children;
		//glm::mat4 localBindTransform;
		glm::mat4 inverseBindTransform; 
		glm::mat4 animationTransform;
	};
}
