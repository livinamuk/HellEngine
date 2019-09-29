#include "hellpch.h"
#include "ShadowCubeMapArray.h"

const float ShadowCubeMapArray::SHADOW_NEAR_PLANE = 0.1f;
const float ShadowCubeMapArray::SHADOW_FAR_PLANE = 10.0f;
const int ShadowCubeMapArray::SHADOW_SIZE = 256;

ShadowCubeMapArray::ShadowCubeMapArray()
{
}

ShadowCubeMapArray::ShadowCubeMapArray(int layers)
{
	// create depth cubemap texture
	glGenFramebuffers(1, &ID);
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubemap);
	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, layers * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "CubeMapArray not complete!" << std::endl;
	//else
	//	std::cout << "CubeMapArray complete." << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


ShadowCubeMapArray::~ShadowCubeMapArray()
{
}
