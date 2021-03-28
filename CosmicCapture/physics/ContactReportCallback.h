#pragma once

#include <physx/PxPhysicsAPI.h>
#include "../audio/AudioEngine.h"

using namespace physx;


class ContactReportCallback : public PxSimulationEventCallback
{
public:

	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override
	{
		PX_UNUSED(constraints);
		PX_UNUSED(count);
	}

	void onWake(PxActor** actors, PxU32 count) override
	{
		PX_UNUSED(actors);
		PX_UNUSED(count);
	}

	void onSleep(PxActor** actors, PxU32 count) override
	{
		PX_UNUSED(actors);
		PX_UNUSED(count);
	}

	void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
};

