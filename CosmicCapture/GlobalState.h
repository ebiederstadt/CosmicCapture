#pragma once

#include <physx/PxPhysicsAPI.h>
#include <optional>
#include <array>
#include <map>
#include "./audio/AudioEngine.h"


using namespace physx;

enum class PowerUpOptions
{
	SPIKE_TRAP, SPEED_BOOST, PROJECTILE
};

struct spikeTrapState
{
	PxRigidStatic* triggerBody = nullptr; // The trigger body for the spike trap
	bool active = false; // Spike trap has been deployed and is ready to catch people
	bool inUse = false; // The spike trap has caught somebody
	int actingUpon; // Which car the spike trap caught
};

struct projectileState
{
	PxRigidDynamic* body = nullptr; // The body of the projectile
	bool active = false; // True when the projectile can blow up cars
};


struct State 
{
	inline static int worldGrid[26][26] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 9, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 9, 1, 0, 1, 1, 0,
		0, 1, 1, 1, 1, 9, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 9, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 9, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 9, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 1, 9, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 9, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

	};
	inline static std::pair<float, float> worldGridCenterCoords[26][26];


	inline static bool flagPickedUp = false;
	inline static std::array<bool, 4> flagPickedUpBy = { false, false, false, false };
	inline static PxRigidStatic* flagPickupBox = nullptr;
	inline static PxRigidDynamic* flagBody = nullptr;
	inline static PxRigidStatic* flagDropoffBoxes[4] = { nullptr, nullptr, nullptr, nullptr };
	inline static bool resetFlag = false;

	inline static PxVehicleDrive4W* vehicles[4] = { nullptr, nullptr, nullptr, nullptr };

	//Human players
	inline static int numHumanPlayers = 0;

	// Scores and powerups
	inline static std::array<int, 4> scores = {0, 0, 0, 0};
	inline static int winScore = 3;
	inline static std::array<std::optional<PowerUpOptions>, 4> heldPowerUps = {};

	inline static std::map<int, PxRigidStatic*> spikeTrapPickupTriggerBodies;
	inline static std::map<int, PxRigidStatic*> projectilePickupTriggerBodies;
	inline static std::map<int, PxRigidStatic*> speedBoostPickupTriggerBodies;

	inline static std::map<int, spikeTrapState> spike_trap_states;
	inline static std::map<int, projectileState> projectileStates;

	inline static std::array<bool, 4> killCars = { false, false, false, false };

	inline static bool blueArena = false;
	inline static bool redArena = true;

	inline static bool canPickupFlag = true;
	inline static bool startPickupFlagTimer = false;
	inline static float flagTimer = 0.0f;

	inline static std::optional<int> slowCar;
};

