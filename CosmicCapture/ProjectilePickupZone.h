#pragma once

#include "Entity.h"
#include "GlobalState.h"
#include "physics/Physics.h"
#include "physics/VehicleFilterShader.h"

class ProjectilePickupZone : public Entity
{
public:
	ProjectilePickupZone(std::shared_ptr<Camera> camera, const PxVec3& location);
	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

private:
	PxRigidStatic* pickupBody;

	int mID;
	PxVec3 mLocation;

	constexpr static float MIN_HEIGHT = 2.0f;
	constexpr static float MAX_HEIGHT = 5.0f;
	bool ascending = true;
	
};

