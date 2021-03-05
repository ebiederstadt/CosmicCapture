#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <openal/al.h>
#include <openal/alc.h>

class AudioInstance
{
public:
	AudioInstance(ALuint* buffer);
	~AudioInstance();
	void initialize();
	void playSound();
	void stopSound();
	bool isSoundPlaying();
	void cleanup();

	ALuint source;
	ALint source_state;
};