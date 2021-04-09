#include "ProjectilePickupZone.h"

ProjectilePickupZone::ProjectilePickupZone(std::shared_ptr<Camera> camera, const PxVec3& location) :
	Entity("models/powerup_stand.obj", "textures/rocket_preview_white.png", camera),
	mLocation(location)
{}


void ProjectilePickupZone::attachPhysics(Physics& instance) {

	mID = static_cast<int>(State::projectilePickupTriggerBodies.size());
	
	PxShape* projectilePickupMarker = instance.gPhysics->createShape(PxSphereGeometry(5.f), *instance.gMaterial, true);
	projectilePickupMarker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
	pickupBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(mLocation.x, MIN_HEIGHT, mLocation.z)));
	pickupBody->attachShape(*projectilePickupMarker);
	projectilePickupMarker->release();
	instance.gScene->addActor(*pickupBody);

	PxShape* projectilePickupTriggerShape = instance.gPhysics->createShape(PxBoxGeometry(1.1f, 2.f, 1.1f), *instance.gMaterial, true);
	//trigger box for picking up the flag
	projectilePickupTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	projectilePickupTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	PxRigidStatic* body = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(mLocation.x, 2.f, mLocation.z)));
	body->attachShape(*projectilePickupTriggerShape);
	instance.gScene->addActor(*body);

	State::projectilePickupTriggerBodies[mID] = body;
}
void ProjectilePickupZone::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) {
	PxTransform transform = pickupBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth, 0);
}

void ProjectilePickupZone::simulate(Physics& instance) {
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
void ProjectilePickupZone::cleanUpPhysics() {
	PX_RELEASE(State::projectilePickupTriggerBodies[mID]);
	PX_RELEASE(pickupBody);
}