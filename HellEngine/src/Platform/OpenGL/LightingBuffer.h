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
		unsigned int gLighting, gComposite, gDOF;

		void Configure(int width, int height);
	};
}