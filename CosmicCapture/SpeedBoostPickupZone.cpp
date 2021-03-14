#include "SpeedBoostPickupZone.h"
#define MIN_HEIGHT 2.0f
#define MAX_HEIGHT 5.0f
bool ascendingSpeedBoost = true;

SpeedBoostPickupZone::SpeedBoostPickupZone(std::shared_ptr<Camera> camera) :
	Entity("models/projectile_sphere.ply", "textures/blue.jpg", camera)
{}

void SpeedBoostPickupZone::attachPhysics(Physics& instance) {
	PxShape* speedboostPickupMarker = instance.gPhysics->createShape(PxSphereGeometry(1.f), *instance.gMaterial, true);
	speedboostPickupMarker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
	State::speedboostPickupMarkerBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(25.f, MIN_HEIGHT, 25.f)));
	State::speedboostPickupMarkerBody->attachShape(*speedboostPickupMarker);
	speedboostPickupMarker->release();
	instance.gScene->addActor(*State::speedboostPickupMarkerBody);

	PxShape* speedboostPickupTriggerShape = instance.gPhysics->createShape(PxBoxGeometry(1.1f, 2.f, 1.1f), *instance.gMaterial, true);
	//trigger box for picking up the flag
	speedboostPickupTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	speedboostPickupTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::speedboostPickupTriggerBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(25.f, 2.f, 25.f)));
	State::speedboostPickupTriggerBody->attachShape(*speedboostPickupTriggerShape);
	instance.gScene->addActor(*State::speedboostPickupTriggerBody);
}

void SpeedBoostPickupZone::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) {
	PxTransform transform = State::speedboostPickupMarkerBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth, depthMap);
}

void SpeedBoostPickupZone::simulate(Physics& instance) {
	PxVec3 pos = State::speedboostPickupMarkerBody->getGlobalPose().p;
	if (ascendingSpeedBoost) {
		State::speedboostPickupMarkerBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y + 0.01f, pos.z)));
		if (pos.y + 0.1f >= MAX_HEIGHT) {
			ascendingSpeedBoost = false;
		}
	}
	else {
		State::speedboostPickupMarkerBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y - 0.01f, pos.z)));
		if (pos.y - 0.1f <= MIN_HEIGHT) {
			ascendingSpeedBoost = true;
		}
	}

}

void SpeedBoostPickupZone::cleanUpPhysics() {
	PX_RELEASE(State::speedboostPickupTriggerBody);
	PX_RELEASE(State::speedboostPickupMarkerBody);
}