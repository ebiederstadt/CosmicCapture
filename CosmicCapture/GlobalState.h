#pragma once

#include <physx/PxPhysicsAPI.h>
#include "Vehicle.h"


using namespace physx;

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

	inline static PxRigidDynamic* vehicleRDs[4] = { nullptr, nullptr, nullptr, nullptr };

	inline static int scores[4] = {0, 0, 0, 0};

	inline static PxRigidDynamic* projectileBody = nullptr;
	inline static PxRigidStatic* projectilePickupTriggerBody = nullptr;
	inline static bool projectilePickedUp = false;

	inline static PxRigidDynamic* speedboostBody = nullptr;
	inline static PxRigidStatic* speedboostPickupTriggerBody = nullptr;
	inline static bool speedboostPickedUp = false;

	inline static bool speedBoostFinished = false;

	inline static PxRigidStatic* spikeTrapPickupTriggerBody = nullptr;
	inline static bool spikeTrapPickedUp = false;

	inline static PxRigidStatic* spikeTrapTriggerBody = nullptr;
	inline static bool spikeTrapActive = false; // Active: The trap is ready for use
	inline static bool spikeTrapInUse = false; // In use: The trap is acting on a car
	inline static int spikeTrapActingUpon = 0; // Which car the spike trap is affecting

	inline static bool spikeTrapFinished = false; // Use when the trap is finished and should be removed
};
