#pragma once

#include <physx/PxPhysicsAPI.h>
#include "Vehicle.h"


using namespace physx;

struct State 
{
	// Flag state
	inline static bool flagPickedUp[4] = { false, false, false, false };
	inline static PxRigidStatic* pickupBox = nullptr; //For flag -- Todo: Rename
	inline static PxRigidDynamic* flagBody = nullptr;
	inline static PxRigidStatic* flagDropoffBoxes[4] = { nullptr, nullptr, nullptr, nullptr };

	inline static PxRigidDynamic* vehicleRDs[4] = { nullptr, nullptr, nullptr, nullptr };

	inline static int scores[4] = {0, 0, 0, 0};

	inline static PxRigidDynamic* projectileBody = nullptr;
	inline static PxRigidStatic* projectilePickupMarkerBody = nullptr;
	inline static PxRigidStatic* projectilePickupTriggerBody = nullptr;
	inline static bool projectilePickedUp = false;
};
