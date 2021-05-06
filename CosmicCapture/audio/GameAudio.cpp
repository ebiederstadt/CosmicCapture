#include "GameAudio.h"

void GameAudio::initialize()
{
	soundSystem.initialize();
	soundSystem.initializeBuffers();
	
	sounds[GameSounds::MUSIC] = soundSystem.createInstance(audioConstants::SOUND_FILE_MAIN_TRACK);
	loop(GameSounds::MUSIC);
	play(GameSounds::MUSIC);

	sounds[GameSounds::ENGINE] = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE);
	loop(GameSounds::ENGINE);
	setVolume(GameSounds::ENGINE, 0.1f);
	play(GameSounds::ENGINE);

	sounds[GameSounds::COLLISION] = soundSystem.createInstance(audioConstants::SOUND_FILE_COLLISION);
	sounds[GameSounds::PROJECTILE] = soundSystem.createInstance(audioConstants::SOUND_FILE_PROJECTILE);
	sounds[GameSounds::FLAG_PICKUP] = soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_PICKUP);
	sounds[GameSounds::PROJECTILE_PICKUP] = soundSystem.createInstance(audioConstants::SOUND_FILE_PROJECTILE_PICKUP);
	sounds[GameSounds::SPIKE_TRAP_PICKUP] = soundSystem.createInstance(audioConstants::SOUND_FILE_SPIKE_TRAP_PICKUP);
	sounds[GameSounds::SPEED_BOOST_PICKUP] = soundSystem.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST_PICKUP);
	sounds[GameSounds::FLAG_RETURN] = soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_RETURN);
	sounds[GameSounds::SPEED_BOOST] = soundSystem.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST);
	sounds[GameSounds::PROJECTILE_EXPLOSION] = soundSystem.createInstance(audioConstants::SOUND_FILE_EXPLOSION);
	sounds[GameSounds::CAR_CRASH] = soundSystem.createInstance(audioConstants::SOUND_FILE_CRASH);
	sounds[GameSounds::FLAG_LOST] = soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_LOST);
	sounds[GameSounds::GATE_SWITCH] = soundSystem.createInstance(audioConstants::SOUND_FILE_GATE_SWITCH);
	sounds[GameSounds::CAUGHT] = soundSystem.createInstance(audioConstants::SOUND_FILE_CAUGHT);

	sounds[GameSounds::ENGINE2] = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE2);
	sounds[GameSounds::ENGINE3] = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE3);
	sounds[GameSounds::ENGINE4] = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE4);
}

void GameAudio::cleanup()
{
	soundSystem.killSources();
}

void GameAudio::play(const GameSounds& sound)
{
	sounds.at(sound).playSound();
}

bool GameAudio::isSoundPlaying(const GameSounds& sound)
{
	return sounds.at(sound).isSoundPlaying();
}

void GameAudio::setVolume(const GameSounds& sound, float amount)
{
	sounds.at(sound).setVolume(amount);
}

void GameAudio::loop(const GameSounds& sound)
{
	sounds.at(sound).loop();
}
