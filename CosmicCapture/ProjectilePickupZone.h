#pragma once

#include "Entity.h"
#include "GlobalState.h"
#include "physics/Physics.h"

class ProjectilePickupZone : public Entity
{
public:
	ProjectilePickupZone(const PxVec3&);
	void attachPhysics(Physics&) override;
	void draw(Physics&, const ShaderProgram&, const Camera&, bool) override;
	void simulate(Physics&) override;
	void cleanUpPhysics() override;

private:
	PxRigidStatic* pickupBody;

	int mID;
	PxVec3 mLocation;

	constexpr static float MIN_HEIGHT = 2.0f;
	constexpr static float MAX_HEIGHT = 5.0f;
	bool ascending = true;
	
};

