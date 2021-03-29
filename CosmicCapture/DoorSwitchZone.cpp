#include "DoorSwitchZone.h"

DoorSwitchZone::DoorSwitchZone(std::shared_ptr<Camera> camera, int doorSwitchZoneN) :
	Entity("models/dropoff_zone.ply", "textures/fancy.jpg", camera)
{
	doorSwitchZoneNum = doorSwitchZoneN;
}

void DoorSwitchZone::attachPhysics(Physics& instance)
{
	float x = 0;
	float z = 0;
	if (doorSwitchZoneNum == 0) {
		x = 124.f;
		z = 124.f;
	}
	else if (doorSwitchZoneNum == 1) {
		x = -124.f;
		z = 124.f;
	}
	else if (doorSwitchZoneNum == 2) {
		x = 124.f;
		z = -124.f;
	}
	else {
		x = -124.f;
		z = -124.f;
	}
	PxShape* doorSwitchZone = instance.gPhysics->createShape(PxBoxGeometry(1.0f, 1.0f, 1.0f), *instance.gMaterial, true);
	//visual indicator for dropoff zone
	doorSwitchZone->setSimulationFilterData(PxFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0));
	State::doorSwitchPickupMarkerBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(x, 0.f, z)));
	State::doorSwitchPickupMarkerBody->attachShape(*doorSwitchZone);
	doorSwitchZone->release();
	instance.gScene->addActor(*State::doorSwitchPickupMarkerBody);
	//----------------------------------------------------------
	//Trigger Shapes--------------------------------------------
	PxShape* doorSwitchDropoffShape = instance.gPhysics->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *instance.gMaterial, true);
	//trigger box for hitting the button
	doorSwitchDropoffShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	doorSwitchDropoffShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::doorSwitchPickupTriggerBody[doorSwitchZoneNum] = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(x, 0.f, z)));
	State::doorSwitchPickupTriggerBody[doorSwitchZoneNum]->attachShape(*doorSwitchDropoffShape);
	instance.gScene->addActor(*State::doorSwitchPickupTriggerBody[doorSwitchZoneNum]);
	
}

void DoorSwitchZone::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth)
{
	// Draw the drop-off box
	PxTransform transform = State::doorSwitchPickupTriggerBody[doorSwitchZoneNum]->getGlobalPose();
	PxMat44 modelMatrix = PxMat44(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth, 1);
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
	PX_RELEASE(State::doorSwitchPickupTriggerBody[doorSwitchZoneNum]);
	PX_RELEASE(State::doorSwitchPickupMarkerBody);
}
