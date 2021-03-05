#include "ProjectilePickupZone.h"
#define MIN_HEIGHT 2.0f
#define MAX_HEIGHT 5.0f
bool ascending = true;

ProjectilePickupZone::ProjectilePickupZone(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera) :
	Entity("models/projectile_sphere.ply", "textures/wall.jpg", shaderProgram, camera)
{}


void ProjectilePickupZone::attachPhysics(Physics& instance) {
	PxShape* projectilePickupMarker = instance.gPhysics->createShape(PxSphereGeometry(1.f), *instance.gMaterial, true);
	projectilePickupMarker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
	State::projectilePickupMarkerBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, MIN_HEIGHT, 25.f)));
	State::projectilePickupMarkerBody->attachShape(*projectilePickupMarker);
	projectilePickupMarker->release();
	instance.gScene->addActor(*State::projectilePickupMarkerBody);

	PxShape* projectilePickupTriggerShape = instance.gPhysics->createShape(PxBoxGeometry(1.1f, 2.f, 1.1f), *instance.gMaterial, true);
	//trigger box for picking up the flag
	projectilePickupTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	projectilePickupTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::projectilePickupTriggerBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 2.f, 25.f)));
	State::projectilePickupTriggerBody->attachShape(*projectilePickupTriggerShape);
	instance.gScene->addActor(*State::projectilePickupTriggerBody);
}
void ProjectilePickupZone::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap) {
	PxTransform transform = State::projectilePickupMarkerBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth, depthMap);
}
void ProjectilePickupZone::simulate(Physics& instance) {
	PxVec3 pos = State::projectilePickupMarkerBody->getGlobalPose().p;
	if (ascending) {
		State::projectilePickupMarkerBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y + 0.01f, pos.z)));
		if (pos.y + 0.1f >= MAX_HEIGHT) {
			ascending = false;
		}
	}
	else {
		State::projectilePickupMarkerBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y - 0.01f, pos.z)));
		if (pos.y - 0.1f <= MIN_HEIGHT) {
			ascending = true;
		}
	}

}
void ProjectilePickupZone::cleanUpPhysics() {
	PX_RELEASE(State::projectilePickupTriggerBody);
	PX_RELEASE(State::projectilePickupMarkerBody);
}