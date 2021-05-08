#pragma once

#include <physx/PxPhysicsAPI.h>

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

	void onTrigger(PxTriggerPair*, PxU32) override;
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {}
	void onContact(const PxContactPairHeader&, const PxContactPair*, PxU32) override;
};

