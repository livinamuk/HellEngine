#pragma once

#include <cstdlib>
#include <iostream>
#include <windows.h>
#include "common.h"
#include "fmod.hpp"
#include <vector>

namespace HellEngine
{
	class Audio
	{
	public:
		Audio(std::string fileName);
		~Audio();
		Audio();


		static std::vector<Audio> loadedAudioFiles;
		static void MainUpdate();
		static void PlayAudio(std::string fileName);
		static void LoadAudio(std::string fileName);
		static void StreamAudio(std::string fileName);

		static Audio stream;

		std::string name;
		FMOD::System* system;
		FMOD::Sound* sound1;
		FMOD::Channel* channel = 0;
		FMOD_RESULT       result;
		unsigned int      version;
		void* extradriverdata = 0;

		// for stream
		FMOD::Sound* sound_to_play;
		int numsubsounds;

		void Update();
		void Close();
		
	

	private:
		void Play();

	};
}