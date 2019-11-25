
#include "hellpch.h"
#include "ShotgunLogic.h"
#include "HellEngine/AssetManager.h"

namespace HellEngine {

	bool ShotgunLogic::firing = false;
	bool ShotgunLogic::shellEjected = false;
	float ShotgunLogic::animationTimer = 0;

	void ShotgunLogic::FireShotgun()
	{
		firing = true;
		shellEjected = false;
		animationTimer = 0;
	}

	void ShotgunLogic::Update(float deltaTime, std::vector<Shell>* shells, SkinnedMesh* skinnedMesh, Camera* camera)
	{
		if (firing)
			animationTimer += deltaTime;

		if (~shellEjected && animationTimer > 0.5f)
		{
			shellEjected = true;

			glm::vec3 position = skinnedMesh->boltPos + (camera->Up * -Shell::shellUpFactor) + (camera->Right * Shell::shellRightFactor) + (camera->Front * -Shell::shellForwardFactor);
			shells->push_back(Shell(AssetManager::GetModelByName("Shell.fbx"), position, camera, deltaTime));

			//Logic::AddToQueue("EJECT_SHOTGUN_SHELL");
		}
	};
}