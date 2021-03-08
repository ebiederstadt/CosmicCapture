#include "SpeedBoost.h"
#include <iostream>

SpeedBoost::SpeedBoost(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera) :
	Entity("models/projectile_sphere.ply", "textures/blue.jpg", shaderProgram, camera)
{}

void SpeedBoost::attachPhysics(Physics& instance)
{
	affectionTimer = 0.0f;
}

void SpeedBoost::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) {}

void SpeedBoost::simulate(Physics& instance) {
	std::cout << mVehicles[0]->mDriveDynData.getEngineRotationSpeed() << std::endl;
	affectionTimer += 1.0f;
	if (affectionTimer <= AFFECTION_TIME)
	{
		mVehicles[0]->mDriveDynData.setEngineRotationSpeed((mVehicles[0]->mDriveDynData.getEngineRotationSpeed() + 500) * 2);
	}
	else
		State::speedBoostFinished = true;
	std::cout << mVehicles[0]->mDriveDynData.getEngineRotationSpeed() << std::endl;
}

void SpeedBoost::cleanUpPhysics() {
	PX_RELEASE(State::speedboostBody);
}