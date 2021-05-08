#include "SpikeTrapPickupZone.h"
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"

SpikeTrapPickupZone::SpikeTrapPickupZone(const PxVec3& location):
	Entity("models/powerup_stand.obj", "textures/spike_preview_white.png"),
	mLocation(location)
{}

void SpikeTrapPickupZone::attachPhysics(Physics& instance)
{
	mID = static_cast<int>(State::spikeTrapPickupTriggerBodies.size());
	
	PxShape* spikeTrapPickupMarker = instance.gPhysics->createShape(PxSphereGeometry(5.f), *instance.gMaterial, true);
	spikeTrapPickupMarker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
	pickupBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(mLocation.x, MIN_HEIGHT, mLocation.z)));
	pickupBody->attachShape(*spikeTrapPickupMarker);
	spikeTrapPickupMarker->release();
	instance.gScene->addActor(*pickupBody);

	PxShape* speedboostPickupTriggerShape = instance.gPhysics->createShape(PxBoxGeometry(1.1f, 2.f, 1.1f), *instance.gMaterial, true);
	speedboostPickupTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	speedboostPickupTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	PxRigidStatic* body = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(mLocation.x, 2.f, mLocation.z)));
	body->attachShape(*speedboostPickupTriggerShape);
	instance.gScene->addActor(*body);

	State::spikeTrapPickupTriggerBodies[mID] = body;
}

void SpikeTrapPickupZone::draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth)
{
	PxTransform transform = pickupBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, shader, camera, depth, 0);
}

void SpikeTrapPickupZone::simulate(Physics& instance)
{
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

void SpikeTrapPickupZone::cleanUpPhysics()
{
	PX_RELEASE(pickupBody);
	PX_RELEASE(State::spikeTrapPickupTriggerBodies[mID]);
}
