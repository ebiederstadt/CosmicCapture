#pragma once

#include <physx/PxPhysicsAPI.h>

using namespace physx;

struct State 
{
	// Flag state
	inline static bool flagPickedUp = false;
	inline static PxRigidStatic* pickupBox = nullptr;
	inline static PxRigidStatic* dropoffBox = nullptr;
	inline static PxRigidDynamic* flagBody = nullptr;
};
