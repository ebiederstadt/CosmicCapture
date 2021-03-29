#include "SpeedBoostPickupZone.h"

using namespace physx;

SpeedBoostPickupZone::SpeedBoostPickupZone(std::shared_ptr<Camera> camera, const PxVec3& location) :
	Entity("models/projectile_sphere.ply", "textures/blue.jpg", camera),
	mLocation(location)
{}

void SpeedBoostPickupZone::attachPhysics(Physics& instance) {

	mID = static_cast<int>(State::speedBoostPickupTriggerBodies.size());
	
	PxShape* speedboostPickupMarker = instance.gPhysics->createShape(PxSphereGeometry(1.f), *instance.gMaterial, true);
	speedboostPickupMarker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
	pickupBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(mLocation.x, MIN_HEIGHT, mLocation.z)));
	pickupBody->attachShape(*speedboostPickupMarker);
	speedboostPickupMarker->release();
	instance.gScene->addActor(*pickupBody);

	PxShape* speedboostPickupTriggerShape = instance.gPhysics->createShape(PxBoxGeometry(1.1f, 2.f, 1.1f), *instance.gMaterial, true);
	//trigger box for picking up the flag
	speedboostPickupTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	speedboostPickupTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	PxRigidStatic* body = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(mLocation.x, 2.f, mLocation.z)));
	body->attachShape(*speedboostPickupTriggerShape);
	instance.gScene->addActor(*body);

	State::speedBoostPickupTriggerBodies[mID] = body;
}

void SpeedBoostPickupZone::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) {
	PxTransform transform = pickupBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth, 1);
}

void SpeedBoostPickupZone::simulate(Physics& instance) {
	PxVec3 pos = pickupBody->getGlobalPose().p;
	if (ascending) {
		pickupBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y + 0.01f, pos.z)));
		if (pos.y + 0.1f >= MAX_HEIGHT) {
			ascending = false;
		}
	}
	else {
		pickupBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y - 0.01f, pos.z)));
		if (pos.y - 0.1f <= MIN_HEIGHT) {
			ascending = true;
		}
	}

}

void SpeedBoostPickupZone::cleanUpPhysics() {
	PX_RELEASE(State::speedBoostPickupTriggerBodies[mID]);
	PX_RELEASE(pickupBody);
}