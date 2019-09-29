#pragma once

namespace HellEngine
{
	class GBuffer
	{
	public:
		GBuffer(int width, int height);
		GBuffer();
		~GBuffer();

		unsigned int ID;
		unsigned int gEmmisive, gNormal, gAlbedoSpec, rboDepth, gMousePick, gLighting;

		void Configure(int width, int height);
	};
}
