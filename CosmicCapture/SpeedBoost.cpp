#include "SpeedBoost.h"
#include <iostream>

SpeedBoost::SpeedBoost(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera) :
	Entity("models/projectile_sphere.ply", "textures/blue.jpg", shaderProgram, camera)
{}

void SpeedBoost::attachPhysics(Physics& instance) {
	std::cout << mVehicles[0]->mDriveDynData.getEngineRotationSpeed() << std::endl;
	mVehicles[0]->mDriveDynData.setEngineRotationSpeed((mVehicles[0]->mDriveDynData.getEngineRotationSpeed()+500) * 2);
	std::cout << mVehicles[0]->mDriveDynData.getEngineRotationSpeed() << std::endl;
}

void SpeedBoost::draw(Physics& instance) {
}

void SpeedBoost::simulate(Physics& instance) {
	
}

void SpeedBoost::cleanUpPhysics() {
	PX_RELEASE(State::speedboostBody);
}