#pragma once

#include "Entity.h"

using namespace physx;

class SpikeTrapPickupZone : public Entity
{
public:
	SpikeTrapPickupZone(const PxVec3&);

	void attachPhysics(Physics&) override;
	void draw(Physics&, const ShaderProgram&, const Camera&, bool) override;
	void simulate(Physics&) override;
	void cleanUpPhysics() override;

private:
	PxRigidStatic* pickupBody;

	int mID;
	physx::PxVec3 mLocation;

	bool ascending = false;

	constexpr static float MIN_HEIGHT = 2.0f;
	constexpr static float MAX_HEIGHT = 5.0f;
};

