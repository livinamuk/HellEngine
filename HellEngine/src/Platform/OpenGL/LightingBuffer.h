#pragma once
#pragma once

namespace HellEngine
{
	class LightingBuffer
	{
	public:
		LightingBuffer(int width, int height);
		LightingBuffer();
		~LightingBuffer();

		unsigned int ID;
		unsigned int rboDepth, gLighting, gFinal;

		void Configure(int width, int height);
	};
}