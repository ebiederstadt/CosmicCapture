#pragma once

#include <vector>
#include <memory>
#include <openal/alc.h>

#include "AudioInstance.h"

namespace audioConstants
{
	// engine (can be instanced multiple times)
	const int SOUND_FILE_ENGINE = 0;
	const int SOUND_FILE_ENGINE2 = 14;
	const int SOUND_FILE_ENGINE3 = 15;
	const int SOUND_FILE_ENGINE4 = 16;

	//music tracks
	const int SOUND_FILE_MAIN_TRACK = 1;

	//other effects
	const int SOUND_FILE_COLLISION = 2;
	const int SOUND_FILE_PROJECTILE = 3;
	const int SOUND_FILE_FLAG_PICKUP = 4;
	const int SOUND_FILE_PROJECTILE_PICKUP = 5;
	const int SOUND_FILE_SPIKE_TRAP_PICKUP = 6;
	const int SOUND_FILE_SPEED_BOOST_PICKUP = 7;
	const int SOUND_FILE_FLAG_RETURN = 8;
	const int SOUND_FILE_SPEED_BOOST = 9;
	const int SOUND_FILE_CRASH = 10;
	const int SOUND_FILE_EXPLOSION = 11;
	const int SOUND_FILE_FLAG_LOST = 12;
	const int SOUND_FILE_GATE_SWITCH = 13;
	const int SOUND_FILE_CAUGHT = 17;

}

class AudioEngine
{
	public:
		void initialize();
		void initializeBuffers(bool engineOnly = false);
		AudioInstance& createInstance(int);
		void killSources() const;

	private:
		static void CheckError();
		static bool _strcmp(const char*, const char*);
		static bool loadWavFile(const char*, ALuint*);

		static const int NUM_TOTAL = 18;
		static const int NUM_EFFECTS = 17;
		static const int NUM_TRACKS = 1;
		const char* soundEffects[NUM_EFFECTS];
		const char* musicTracks[NUM_TRACKS];

		ALuint bufferArray[NUM_TOTAL];
		ALuint sources[NUM_TOTAL];
		std::vector<std::unique_ptr<AudioInstance>> listOfSources;

		ALCdevice* device;
		ALCcontext* context;
};