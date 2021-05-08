#include "Projectile.h"
#include "physics/VehicleFilterShader.h"

extern float scalingFactor;

Projectile::Projectile() :
	Entity("models/rocket.obj", "textures/rocket_texture.png")
{
	// Initialize the state without initializing the physx body
	projectileState state;
	mID = static_cast<int>(State::projectileStates.size());

	// Check to see if the ID we are using is already being used by somebody else
	auto search = State::projectileStates.find(mID);
	while (search != State::projectileStates.end())
	{
		mID += 1;
		search = State::projectileStates.find(mID);
	}
	
	State::projectileStates[mID] = state;
}

void Projectile::attachPhysics(Physics& instance) {
	PxShape* projectile = instance.gPhysics->createShape(PxSphereGeometry(1.f), *instance.gMaterial, true); //create projectile shape
	projectile->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
	PxVec3 pos = mVehicle->getRigidDynamicActor()->getGlobalPose().p;
	PxVec3 dir = mVehicle->getRigidDynamicActor()->getLinearVelocity();

	State::projectileStates[mID].body = instance.gPhysics->createRigidDynamic(PxTransform(pos + dir.getNormalized() * 3.0f));
	State::projectileStates[mID].body->attachShape(*projectile);
	projectile->release();
	State::projectileStates[mID].body->setAngularDamping(0.0f);
	State::projectileStates[mID].body->setLinearVelocity(PxVec3(dir.x * scalingFactor, dir.y * scalingFactor, dir.z * scalingFactor));
	instance.gScene->addActor(*State::projectileStates[mID].body);
}

void Projectile::draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) {

	PxTransform transform = State::projectileStates[mID].body->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, shader, camera, depth, 2);
}

void Projectile::simulate(Physics& instance) {
	if (!State::projectileStates[mID].active)
	{
		activationTimer += 1.0f;
		if (activationTimer >= ACTIVATION_TIME)
			State::projectileStates[mID].active = true;
	}
	else 
	{
		removalTimer += 1.0f;
		if (removalTimer >= REMOVAL_TIME)
			shouldBeDeleted = true;
	}
}

void Projectile::cleanUpPhysics() {
	if (State::projectileStates[mID].body != nullptr)
		PX_RELEASE(State::projectileStates[mID].body);
	State::projectileStates.erase(mID);
}