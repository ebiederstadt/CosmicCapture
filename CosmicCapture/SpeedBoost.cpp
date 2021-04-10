#include "SpeedBoost.h"
#include <iostream>

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
		//std::cout << mVehicles[0]->mDriveDynData.getEngineRotationSpeed() << std::endl;
		affectionTimer += 1.0f;
		if (affectionTimer <= AFFECTION_TIME)
		{
			mVehicles[0]->mDriveDynData.setEngineRotationSpeed((mVehicles[0]->mDriveDynData.getEngineRotationSpeed() + 500) * 2);
		}
		else
		{
			active = false;
			shouldBeDeleted = true;
		}
		//std::cout << mVehicles[0]->mDriveDynData.getEngineRotationSpeed() << std::endl;
	}
}

void SpeedBoost::cleanUpPhysics() {}
