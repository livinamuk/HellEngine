#pragma once
class ShadowCubeMapArray
{
public:
	ShadowCubeMapArray();
	ShadowCubeMapArray(int layers);
	~ShadowCubeMapArray();

	unsigned int ID;
	unsigned int depthCubemap;

	const static float SHADOW_NEAR_PLANE;
	const static float SHADOW_FAR_PLANE;
	const static int SHADOW_SIZE;
};

