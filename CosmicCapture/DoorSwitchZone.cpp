#include "DoorSwitchZone.h"

DoorSwitchZone::DoorSwitchZone(std::shared_ptr<Camera> camera) :
	Entity("models/dropoff_zone.ply", "textures/fancy.jpg", camera)
{
}

void DoorSwitchZone::attachPhysics(Physics& instance)
{
	PxShape* doorSwitchZone = instance.gPhysics->createShape(PxBoxGeometry(1.0f, 1.0f, 1.0f), *instance.gMaterial, true);
	//visual indicator for dropoff zone
	doorSwitchZone->setSimulationFilterData(PxFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0));
	State::doorSwitchPickupMarkerBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(20.f, 0.f, 0.f)));
	State::doorSwitchPickupMarkerBody->attachShape(*doorSwitchZone);
	doorSwitchZone->release();
	instance.gScene->addActor(*State::doorSwitchPickupMarkerBody);
	//----------------------------------------------------------
	//Trigger Shapes--------------------------------------------
	PxShape* doorSwitchDropoffShape = instance.gPhysics->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *instance.gMaterial, true);
	//trigger box for hitting the button
	doorSwitchDropoffShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	doorSwitchDropoffShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::doorSwitchPickupTriggerBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(20.f, 0.f, 0.f)));
	State::doorSwitchPickupTriggerBody->attachShape(*doorSwitchDropoffShape);
	instance.gScene->addActor(*State::doorSwitchPickupTriggerBody);
	
}

void DoorSwitchZone::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth)
{
	// Draw the drop-off box
	PxTransform transform = State::doorSwitchPickupMarkerBody->getGlobalPose();
	PxMat44 modelMatrix = PxMat44(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth);
}

void DoorSwitchZone::simulate(Physics& instance)
{
	if (!State::arenaSwitchReady)
	{
		State::arenaTimer += 1;
		if (State::arenaTimer >= AFFECTION_TIME)
		{
			State::arenaSwitchReady = true;
			fmt::print("arena switch ready\n");
		}
	}
}

void DoorSwitchZone::cleanUpPhysics()
{
	PX_RELEASE(State::doorSwitchPickupTriggerBody);
	PX_RELEASE(State::doorSwitchPickupMarkerBody);
}
