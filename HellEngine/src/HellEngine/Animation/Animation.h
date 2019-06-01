#pragma once
#include "HellEngine/Animation/KeyFrameList.h"

namespace HellEngine {

	class Animation
	{
	public: // methods
		Animation();
		~Animation() = default;

	public: // fields
		float lengthInSeconds; 
		std::vector<KeyFrameList> keyFrameLists; // One for each joint

	private: // methods
		

	private: // fields
	};

}
