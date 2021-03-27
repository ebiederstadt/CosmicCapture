#include "Projectile.h"

extern float scalingFactor;

Projectile::Projectile(std::shared_ptr<Camera> camera) :
	Entity("models/rocket.obj", "textures/rocket_texture.png", camera)
{}

void Projectile::attachPhysics(Physics& instance) {
	PxShape* projectile = instance.gPhysics->createShape(PxSphereGeometry(1.f), *instance.gMaterial, true); //create projectile shape
	projectile->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
	PxVec3 pos = mVehicle->getRigidDynamicActor()->getGlobalPose().p;
	PxVec3 dir = mVehicle->getRigidDynamicActor()->getLinearVelocity();
	mBody = instance.gPhysics->createRigidDynamic(PxTransform(PxVec3(pos.x, pos.y + 5.f, pos.z)));
	mBody->attachShape(*projectile);
	projectile->release();
	mBody->setAngularDamping(0.0f); //I failed highschool physics idk what this means
	mBody->setLinearVelocity(PxVec3(dir.x * scalingFactor, dir.y * scalingFactor, dir.z * scalingFactor));
	instance.gScene->addActor(*mBody);

	active = true;
}

void Projectile::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) {
	PxTransform transform = mBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth);
}

void Projectile::simulate(Physics& instance) {
	if (active)
	{
		activationTimer += 1.0f;
		if (activationTimer >= ACTIVATION_TIME)
		{
			active = false;
			shouldBeDeleted = true;
		}
	}
}

void Projectile::cleanUpPhysics() {
	PX_RELEASE(mBody);
}