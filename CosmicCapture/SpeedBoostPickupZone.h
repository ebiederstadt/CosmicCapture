#pragma once
#include "Entity.h"
#include "GlobalState.h"
#include "physics/Physics.h"
#include "physics/VehicleFilterShader.h"

class SpeedBoostPickupZone : public Entity {
public:
	SpeedBoostPickupZone(const physx::PxVec3& pos);
	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

private:
	PxRigidStatic* pickupBody;

	int mID;
	physx::PxVec3 mLocation;

	constexpr static float MIN_HEIGHT = 2.0f;
	constexpr static float MAX_HEIGHT = 5.0f;
	bool ascending = true;
};