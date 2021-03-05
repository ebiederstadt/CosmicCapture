#include "FlagDropoffZone.h"

FlagDropoffZone::FlagDropoffZone(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera, int player) :
	Entity("models/dropoff_zone.ply", "textures/dropflaghere.jpg", shaderProgram, camera)
{
	playerNum = player;
}

void FlagDropoffZone::attachPhysics(Physics& instance)
{
	float x = 0;
	float z = 0;
	if (playerNum == 0) {
		x = 65.f;
		z = 65.f;
	}
	else if (playerNum == 1) {
		x = -65.f;
		z = 65.f;
	}
	else if (playerNum == 2) {
		x = 65.f;
		z = -65.f;
	}
	else {
		x = -65.f;
		z = -65.f;
	}

	PxShape* dropoffZone = instance.gPhysics->createShape(PxBoxGeometry(1.0f, 0.1f, 1.0f), *instance.gMaterial, true);
	//visual indicator for dropoff zone
	dropoffZone->setSimulationFilterData(PxFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0));
	PxRigidStatic* dropoffZoneBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(x, 0.f, z)));
	dropoffZoneBody->attachShape(*dropoffZone);
	dropoffZone->release();
	instance.gScene->addActor(*dropoffZoneBody);
	//----------------------------------------------------------
	//Trigger Shapes--------------------------------------------
	PxShape* dropoffShape = instance.gPhysics->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *instance.gMaterial, true);
	//trigger box for dropping off the flag
	dropoffShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	dropoffShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::flagDropoffBoxes[playerNum] = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(x, 1.f, z)));
	State::flagDropoffBoxes[playerNum]->attachShape(*dropoffShape);
	instance.gScene->addActor(*State::flagDropoffBoxes[playerNum]);
}

void FlagDropoffZone::draw(Physics& instance)
{
	// Draw the drop-off box
	PxTransform transform = State::flagDropoffBoxes[playerNum]->getGlobalPose();
	PxMat44 modelMatrix = PxMat44(transform);
	mGeometry->draw(modelMatrix);
}

void FlagDropoffZone::simulate(Physics& instance)
{

}

void FlagDropoffZone::cleanUpPhysics()
{
	PX_RELEASE(State::flagDropoffBoxes[playerNum]);
}
