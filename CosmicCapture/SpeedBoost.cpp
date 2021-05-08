#include "SpeedBoost.h"

SpeedBoost::SpeedBoost() :
	Entity("models/projectile_sphere.ply", "textures/blue.jpg")
{}

void SpeedBoost::attachPhysics(Physics& instance)
{
	affectionTimer = 0.0f;
	active = true;
}

void SpeedBoost::draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) {}

void SpeedBoost::simulate(Physics& instance) {
	if (active)
	{
		affectionTimer += 1.0f;
		if (affectionTimer <= AFFECTION_TIME)
		{
			mVehicles[0]->getRigidDynamicActor()->addForce(mVehicles[0]->getRigidDynamicActor()->getLinearVelocity()*1.5f, PxForceMode::eVELOCITY_CHANGE, true);
			fmt::print("{}\n", mVehicles[0]->computeForwardSpeed());
		}
		else
		{
			active = false;
			shouldBeDeleted = true;
		}
	}
}

void SpeedBoost::cleanUpPhysics() {}
