#pragma once

#include "Entity.h"

using namespace physx;

class SpikeTrapPickupZone : public Entity
{
public:
	SpikeTrapPickupZone(const std::shared_ptr<Camera>& camera);

	void attachPhysics(Physics& instance) override;
	void draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) override;
	void simulate(Physics& instance) override;
	void cleanUpPhysics() override;

private:
	PxRigidStatic* pickupBody;

	bool ascending = false;

	constexpr static float MIN_HEIGHT = 2.0f;
	constexpr static float MAX_HEIGHT = 5.0f;
};

