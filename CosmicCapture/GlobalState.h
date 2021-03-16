#pragma once

#include <physx/PxPhysicsAPI.h>
#include <optional>
#include <array>

using namespace physx;

enum class PowerUpOptions
{
	SPIKE_TRAP, SPEED_BOOST, PROJECTILE
};

struct spikeTrapState
{
	inline static PxRigidStatic* triggerBody; // The trigger body for the spike trap
	bool active = false; // Spike trap has been deployed and is ready to catch people
	bool inUse = false; // The spike trap has caught somebody
	int actingUpon; // Which car the spike trap caught
	bool finished = false; // The spike trap is finished and should be removed
};


struct State 
{
	inline static int worldGrid[20][20] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
		1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
		1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1

	};

	inline static bool targetReached = false;
	inline static std::pair<int, int> lastPos;
	inline static std::pair<int, int> stallPos;

	inline static bool flagPickedUp = false;
	inline static bool flagPickedUpBy[4] = { false, false, false, false };
	inline static PxRigidStatic* pickupBox = nullptr; //For flag -- Todo: Rename
	inline static PxRigidDynamic* flagBody = nullptr;
	inline static PxRigidStatic* flagDropoffBoxes[4] = { nullptr, nullptr, nullptr, nullptr };

	inline static PxVehicleDrive4W* vehicles[4] = { nullptr, nullptr, nullptr, nullptr };

	// Scores and powerups
	inline static int scores[4] = {0, 0, 0, 0};
	inline static std::array<std::optional<PowerUpOptions>, 4> heldPowerUps = {};

	inline static PxRigidStatic* projectilePickupTriggerBody = nullptr;
	inline static PxRigidStatic* speedboostPickupTriggerBody = nullptr;
	inline static PxRigidStatic* spikeTrapPickupTriggerBody = nullptr;

	inline static PxRigidDynamic* projectileBody = nullptr;

	inline static bool speedBoostFinished = false;

	inline static std::vector<spikeTrapState> spike_trap_states;
};
