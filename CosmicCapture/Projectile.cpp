#include "Projectile.h"

Projectile::Projectile(std::shared_ptr<Camera> camera) :
	Entity("models/rocket.obj", "textures/blue.jpg", camera)
{}

void Projectile::attachPhysics(Physics& instance) {
	PxShape* projectile = instance.gPhysics->createShape(PxSphereGeometry(1.f), *instance.gMaterial, true); //create projectile shape
	projectile->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
	PxVec3 pos = mVehicle->getRigidDynamicActor()->getGlobalPose().p;
	PxVec3 dir = mVehicle->getRigidDynamicActor()->getLinearVelocity();
	State::projectileBody = instance.gPhysics->createRigidDynamic(PxTransform(PxVec3(pos.x, pos.y + 5.f, pos.z)));
	State::projectileBody->attachShape(*projectile);
	projectile->release();
	State::projectileBody->setAngularDamping(0.0f); //I failed highschool physics idk what this means
	State::projectileBody->setLinearVelocity(PxVec3(dir.x * 10, dir.y * 10, dir.z * 10));
	instance.gScene->addActor(*State::projectileBody);
}

void Projectile::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) {
	PxTransform transform = State::projectileBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth);
}

void Projectile::simulate(Physics& instance) {}

void Projectile::cleanUpPhysics() {
	PX_RELEASE(State::projectileBody);
}