#include "FlagDropoffZone.h"

FlagDropoffZone::FlagDropoffZone(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera, int player) :
	Entity("models/dropoff_zone.ply", "textures/dropflaghere.jpg", shaderProgram, camera),
	playerNum(player)
{}

void FlagDropoffZone::attachPhysics(Physics& instance)
{
	PxShape* dropoffZone = instance.gPhysics->createShape(PxBoxGeometry(1.0f, 0.1f, 1.0f), *instance.gMaterial, true);
	//visual indicator for dropoff zone
	dropoffZone->setSimulationFilterData(PxFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0));
	PxRigidStatic* dropoffZoneBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 0.f, 0.f)));
	dropoffZoneBody->attachShape(*dropoffZone);
	dropoffZone->release();
	instance.gScene->addActor(*dropoffZoneBody);
	//----------------------------------------------------------
	//Trigger Shapes--------------------------------------------
	PxShape* dropoffShape = instance.gPhysics->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *instance.gMaterial, true);
	//trigger box for dropping off the flag
	dropoffShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	dropoffShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::dropoffBox = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 1.f, 0.f)));
	State::dropoffBox->attachShape(*dropoffShape);
	instance.gScene->addActor(*State::dropoffBox);
}

void FlagDropoffZone::draw(Physics& instance)
{
	// Draw the drop-off box
	PxTransform transform = State::dropoffBox->getGlobalPose();
	PxMat44 modelMatrix = PxMat44(transform);
	mGeometry->draw(modelMatrix);
}

void FlagDropoffZone::simulate(Physics& instance)
{

}

void FlagDropoffZone::cleanUpPhysics()
{
	PX_RELEASE(State::dropoffBox);
}
