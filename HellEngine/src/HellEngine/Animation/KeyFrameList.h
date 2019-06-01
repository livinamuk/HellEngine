#pragma once
#include "HellEngine/Animation/JointTransform.h"

namespace HellEngine 
{
	class KeyFrameList
	{
	public: // methods
		KeyFrameList();
		~KeyFrameList();

	public: // fields
		//float timeStamp;
		std::vector<JointTransform> jointTransforms;
	};
}