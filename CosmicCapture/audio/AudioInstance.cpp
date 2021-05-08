#include <openal/al.h>
#include "AudioInstance.h"

AudioInstance::AudioInstance(const ALuint* buffer)
{
	source = *buffer;
	source_state = AL_INITIAL;
}

void AudioInstance::loop() const
{
	alSourcei(source, AL_LOOPING, AL_TRUE);
}

void AudioInstance::playSound() const
{
	alSourcePlay(source);
}

void AudioInstance::setVolume(float volume) const
{
	if (volume > 0.3f) volume = 0.3f;
	alSourcef(source, AL_GAIN, volume);
}

void AudioInstance::stopSound() const
{
	alSourcePause(source);
}

// Check if the state is that the sound is playing 
bool AudioInstance::isSoundPlaying()
{
	alGetSourcei(source, AL_SOURCE_STATE, &source_state);
	return (source_state == AL_PLAYING);
}

