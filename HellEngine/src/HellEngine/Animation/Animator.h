#pragma once
#include "HellEngine/Animation/Joint.h"
#include "HellEngine/Animation/Animation.h"

namespace HellEngine {

	class Animator
	{
	public: // methods
		Animator();
		~Animator() = default;
		void Update(Animation* animation, Joint* joint, glm::mat4 parentTransform, float deltaTime);
		void IncreaseAnimationTime(Animation* animation, float deltaTime);
		float CalculateProgression(Animation* animation);
		void ApplyPoseToJoints(Animation* animation, Joint* joint, glm::mat4 parentTransform, float deltaTime);
		glm::mat4 Animator::interpolate(const JointTransform& matA, const JointTransform& matB, float amount);

	public: // fields

	private: // methods

	private: // fields
	};

}
