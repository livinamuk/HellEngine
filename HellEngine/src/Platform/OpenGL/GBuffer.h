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
		unsigned int gUNUSED, gNormal, gAlbedoSpec, rboDepth, gMousePick;

		void Configure(int width, int height);
	};
}
