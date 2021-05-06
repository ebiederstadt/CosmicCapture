#pragma once

#include <map>

#include "AudioEngine.h"

enum class GameSounds
{
	MUSIC, ENGINE, COLLISION, PROJECTILE,
	FLAG_PICKUP, PROJECTILE_PICKUP, SPIKE_TRAP_PICKUP,
	SPEED_BOOST_PICKUP, FLAG_RETURN, SPEED_BOOST, PROJECTILE_EXPLOSION,
	CAR_CRASH, FLAG_LOST, GATE_SWITCH, CAUGHT, ENGINE2, ENGINE3, ENGINE4
};

class GameAudio
{
public:
	static void initialize();
	static void cleanup();

	static void play(const GameSounds& sound);
	static bool isSoundPlaying(const GameSounds& sound);
	static void setVolume(const GameSounds& sound, float amount);
	static void loop(const GameSounds& sound);

private:
	inline static AudioEngine soundSystem;
	inline static std::map<GameSounds, AudioInstance> sounds;
};

