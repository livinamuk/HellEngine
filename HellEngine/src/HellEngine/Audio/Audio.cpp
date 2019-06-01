#include "hellpch.h"
#include "Audio.h"

namespace HellEngine
{
	Audio::Audio()
	{
		ALCdevice *device;

		device = alcOpenDevice(NULL);
		if (!device)
			std::cout << "ERROR CREATING AUDIO DEVICE!!! \n";
	}


	Audio::~Audio()
	{
	}
}
