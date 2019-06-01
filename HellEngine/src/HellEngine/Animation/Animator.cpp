#include "hellpch.h"
#include "Animator.h"
#include "HellEngine/Util.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/matrix_interpolation.hpp"

namespace HellEngine {

	Animator::Animator()
	{
	}

	float animationTime = 0;
	int previousFrameIndex = 0;
	int nextFrameIndex = 1;

	void Animator::Update(Animation* animation, Joint* joint, glm::mat4 parentTransform, float deltaTime)
	{
		IncreaseAnimationTime(animation, deltaTime);

		if (animationTime == 0) {
			previousFrameIndex = 0;
			nextFrameIndex = 1;
		}

		float nextTimeStamp = animation->keyFrameLists[joint->index].jointTransforms[nextFrameIndex].timeStamp;

		if (animationTime >= nextTimeStamp) {
			previousFrameIndex++;
			nextFrameIndex++;
		}

		float progression = CalculateProgression(animation);
		ApplyPoseToJoints(animation, joint, glm::mat4(1), progression);
	}

	void Animator::IncreaseAnimationTime(Animation* animation, float deltaTime)
	{
		animationTime += deltaTime;
		if (animationTime > animation->lengthInSeconds)
			animationTime = 0;
	}

	float Animator::CalculateProgression(Animation* animation)
	{
		float previousTimeStamp = animation->keyFrameLists[0].jointTransforms[previousFrameIndex].timeStamp;
		float nextTimeStamp = animation->keyFrameLists[0].jointTransforms[nextFrameIndex].timeStamp;

		float totalTime = nextTimeStamp - previousTimeStamp;
		float currentTime = animationTime - previousTimeStamp;

		return currentTime / totalTime;
	}

	void Animator::ApplyPoseToJoints(Animation* animation, Joint* joint, glm::mat4 parentTransform, float amount)
	{
		KeyFrameList thisJointKeyFrameList = animation->keyFrameLists[joint->index];
;
		glm::mat4 currentLocalTransform = this->interpolate(thisJointKeyFrameList.jointTransforms[previousFrameIndex], thisJointKeyFrameList.jointTransforms[nextFrameIndex], amount);
		glm::mat4 currentTransform = parentTransform * currentLocalTransform;

		for (Joint & child : joint->children) {
			ApplyPoseToJoints(animation, &child, currentTransform, amount);
		}

		currentTransform = currentTransform * joint->inverseBindTransform;
		joint->animationTransform = currentTransform;
	}

	glm::mat4 Animator::interpolate(const JointTransform& matA, const JointTransform& matB, float amount)
	{
		auto finalTranslation = glm::mix(matA.position, matB.position, amount);
		auto finalRotation = glm::slerp(matA.rotation, matB.rotation, amount);

		glm::mat4 finalPoseTransform = glm::translate(glm::mat4(1.0f), finalTranslation) * glm::toMat4(finalRotation);

		return finalPoseTransform;
	}

}
