#pragma once
namespace HellEngine 
{
	class JointTransform
	{
	public: // methods
		JointTransform();
		~JointTransform();
		glm::mat4 getLocalTransform();

	public: // fields
		glm::vec3 position;
		glm::quat rotation;
		std::string jointName;
		float timeStamp;
	};
}