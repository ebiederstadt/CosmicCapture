#pragma once

#include <openal/al.h>

class AudioInstance
{
public:
	AudioInstance(const ALuint* buffer);
	void loop() const;
	void playSound() const;
	void setVolume(float volume) const;
	void stopSound() const;
	bool isSoundPlaying();
	static void cleanup() {}

private:
	ALuint source;
	ALint source_state;
};