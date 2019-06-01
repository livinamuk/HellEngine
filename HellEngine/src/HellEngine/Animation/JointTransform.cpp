#include "hellpch.h"
#include "JointTransform.h"
#include "HellEngine/Util.h"
#include "glm/gtx/matrix_interpolation.hpp"
#include "glm/gtx/quaternion.hpp"

namespace HellEngine 
{
	JointTransform::JointTransform()
	{
	}


	JointTransform::~JointTransform()
	{
	}

	glm::mat4 JointTransform::getLocalTransform()
	{
		return glm::translate(glm::mat4(1), position) * glm::toMat4(rotation);
	}

	
}
