#pragma once
#include "HellEngine/Logic/Logic.hpp"
#include "HellEngine/GameObjects/Shell.h"
#include "HellEngine/Animation/SkinnedMesh.h"
#include "HellEngine/Camera.h"

namespace HellEngine
{
	class ShotgunLogic
	{
		// Fields
	public:
		static bool firing;
		static bool shellEjected;
		static float animationTimer;

		// Functions
	public:
		static void FireShotgun();
		static void Update(float deltaTime, std::vector<Shell>* shells, SkinnedMesh* skinnedMesh, Camera* camera)
	};
}