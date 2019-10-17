#include "hellpch.h"
#include "Audio.h"
#include "HellEngine/Util.h"

namespace HellEngine
{
	std::vector<Audio> Audio::loadedAudioFiles;
	Audio Audio::stream;
	FMOD::System* Audio::system;
	void* Audio::extradriverdata = 0;
	FMOD_RESULT Audio::result;

	void Audio::Init()
	{
		Common_Init(&extradriverdata);
		result = FMOD::System_Create(&system);
		ERRCHECK(result);
		//HELL_ERROR("FMOD: " + fileName + " " + std::to_string(result));

		result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
		ERRCHECK(result);
		//HELL_ERROR("FMOD: " + fileName + " " + std::to_string(result));
	}
	Audio::Audio(std::string fileName)
	{	
		name = fileName;
	}

	Audio::~Audio()
	{
	
	}
	Audio::Audio()
	{
	}

	void Audio::LoadAudio(std::string fileName)
	{
		loadedAudioFiles.push_back(Audio(fileName));
	}

	void Audio::MainUpdate()
	{
		for (Audio audio : loadedAudioFiles)
			audio.Update();
	}

	void Audio::PlayAudio(std::string name)
	{
		for (Audio audio : loadedAudioFiles) {
			if (audio.name == name)
				audio.Play();
		}
	}

	void Audio::StreamAudio(std::string name)
	{
		Common_Init(&stream.extradriverdata);
		stream.result = FMOD::System_Create(&stream.system);
		ERRCHECK(stream.result);
		stream.result = stream.system->init(32, FMOD_INIT_NORMAL, stream.extradriverdata);
		ERRCHECK(stream.result);

		std::string path = "res/audio/" + name;
		stream.result = stream.system->createStream(path.c_str(), FMOD_LOOP_NORMAL | FMOD_2D, 0, &stream.sound1);
		ERRCHECK(stream.result);

		stream.result = stream.sound1->getNumSubSounds(&stream.numsubsounds);
		ERRCHECK(stream.result);

		if (stream.numsubsounds)
		{
			stream.sound1->getSubSound(0, &stream.sound_to_play);
			ERRCHECK(stream.result);
		}
		else
		{
			stream.sound_to_play = stream.sound1;
		}

		stream.result = stream.system->playSound(stream.sound_to_play, 0, false, &stream.channel);
		ERRCHECK(stream.result);
	}
	
	void Audio::Play()
	{
		std::string path = "res/audio/" + name;
		result = system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound1);
		ERRCHECK(result);
		result = sound1->setMode(FMOD_LOOP_OFF);
		ERRCHECK(result);
		result = system->playSound(sound1, 0, false, &channel);
		ERRCHECK(result);
	}

	void Audio::Close()
	{
		result = sound1->release();
		ERRCHECK(result);
		result = system->close();
		ERRCHECK(result);
		result = system->release();
		ERRCHECK(result);
		Common_Close();
	}

	void Audio::Update()
	{

		//return;

			result = system->update();
			ERRCHECK(result);

			{
				unsigned int ms = 0;
				unsigned int lenms = 0;
				bool         playing = 0;
				bool         paused = 0;
				int          channelsplaying = 0;

				if (channel)
				{
					FMOD::Sound* currentsound = 0;

					result = channel->isPlaying(&playing);
					if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
					{
						ERRCHECK(result);
					}

					result = channel->getPaused(&paused);
					if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
					{
						ERRCHECK(result);
					}

					result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
					if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
					{
						ERRCHECK(result);
					}

					channel->getCurrentSound(&currentsound);
					if (currentsound)
					{
						result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
						if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
						{
							ERRCHECK(result);
						}
					}
				}
				system->getChannelsPlaying(&channelsplaying, NULL);
			}
	}
}
