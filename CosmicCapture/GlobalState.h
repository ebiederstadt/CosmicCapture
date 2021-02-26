#pragma once

#include <physx/PxPhysicsAPI.h>

using namespace physx;

struct State 
{
	// Flag state
	inline static bool flagPickedUp = false;
	inline static PxRigidStatic* pickupBox = nullptr; //For flag -- Todo: Rename
	inline static PxRigidStatic* dropoffBox = nullptr; //For flag -- Todo: Rename
	inline static PxRigidDynamic* flagBody = nullptr;

	inline static PxRigidDynamic* projectileBody = nullptr;
	inline static PxRigidStatic* projectilePickupMarkerBody = nullptr;
	inline static PxRigidStatic* projectilePickupTriggerBody = nullptr;
	inline static bool projectilePickedUp = false;
};
