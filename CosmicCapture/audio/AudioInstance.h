#pragma once

#include <openal/al.h>

class AudioInstance
{
public:
	AudioInstance() = default;
	AudioInstance(const ALuint*);
	void loop() const;
	void playSound() const;
	void setVolume(float) const;
	void stopSound() const;
	bool isSoundPlaying();
	static void cleanup() {}

private:
	ALuint source;
	ALint source_state;
};