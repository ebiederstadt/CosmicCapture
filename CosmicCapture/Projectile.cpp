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

	mBody = instance.gPhysics->createRigidDynamic(PxTransform(pos + dir.getNormalized() * 3.0f));
	mBody->attachShape(*projectile);
	projectile->release();
	mBody->setAngularDamping(0.0f); //I failed highschool physics idk what this means
	mBody->setLinearVelocity(PxVec3(dir.x * scalingFactor, dir.y * scalingFactor, dir.z * scalingFactor));
	instance.gScene->addActor(*mBody);

	deployed = true;
}

void Projectile::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth) {

	if (active)
	{
		PxTransform transform = State::projectileList[mID]->getGlobalPose();
		PxMat44 modelMatrix(transform);
		mGeometry->draw(modelMatrix, depthTexture, depth, 2);
	}

}

void Projectile::simulate(Physics& instance) {
	if (deployed && !active)
	{
		activationTimer += 1.0f;
		if (activationTimer >= ACTIVATION_TIME)
		{
			active = true;
			mID = static_cast<int>(State::projectileList.size());
			State::projectileList[mID] = mBody;
		}
	}
	
	if (active)
	{
		removalTimer += 1.0f;
		if (removalTimer >= REMOVAL_TIME)
		{
			deployed = false;
			shouldBeDeleted = true;
		}
	}
}

void Projectile::cleanUpPhysics() {
	PX_RELEASE(State::projectileList[mID]);

	State::projectileList.erase(mID);
}