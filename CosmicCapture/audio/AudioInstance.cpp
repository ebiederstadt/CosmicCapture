#define _CRT_SECURE_NO_WARNINGS
#include <openal/al.h>
#include <openal/alc.h>
#include "AudioInstance.h" 
#include "../Globalstate.h"

AudioInstance::AudioInstance(ALuint* buffer) {
	
		source = *buffer;
		source_state = AL_INITIAL;
	}
AudioInstance::~AudioInstance() {
	
	}
	void AudioInstance::loop() {
	
		alSourcei(source, AL_LOOPING, AL_TRUE);
	}
	void AudioInstance::initialize() {
	
	}
	void AudioInstance::playSound() {
		
		alSourcePlay(source);
	}
	void AudioInstance::setVolume(float volume) {

		if(volume > 0.15f && State::numHumanPlayers > 1) volume = 0.15f;
		else if (volume > 0.3f) volume = 0.3f;
		alSourcef(source, AL_GAIN, volume);
	}
	void AudioInstance::AudioInstance::stopSound() {
	
		alSourcePause(source);
	}
	// Check if the state is that the sound is playing 
	bool AudioInstance::isSoundPlaying() {
		
		ALenum state;        
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);        
		return (source_state == AL_PLAYING);
	}

	void AudioInstance::cleanup() {
	
	}
