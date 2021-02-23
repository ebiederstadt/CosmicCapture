#include "Flag.h"

#include <memory>
#include "physics/VehicleFilterShader.h"


Flag::Flag(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera) :
	Entity("models/flag.ply", "textures/blank.jpg", shaderProgram, camera),
	mDropOffZone(std::make_unique<Model>("models/dropoff_zone.ply", "textures/dropflaghere.jpg", shaderProgram, camera))
{}

void Flag::attachPhysics(Physics& instance)
{
	PxShape* flag = instance.gPhysics->createShape(PxBoxGeometry(0.1f, 2.f, 0.1f), *instance.gMaterial, true); //create flag shape
	flag->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
	State::flagBody = instance.gPhysics->createRigidDynamic(PxTransform(PxVec3(-10.f, 2.f, -12.f)));
	//create static rigid body - wont move
	State::flagBody->attachShape(*flag);
	flag->release();
	instance.gScene->addActor(*State::flagBody);

	PxShape* dropoffZone = instance.gPhysics->createShape(PxBoxGeometry(1.0f, 0.1f, 1.0f), *instance.gMaterial, true);
	//visual indicator for dropoff zone
	dropoffZone->setSimulationFilterData(PxFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0));
	PxRigidStatic* dropoffZoneBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 0.f, 0.f)));
	dropoffZoneBody->attachShape(*dropoffZone);
	dropoffZone->release();
	instance.gScene->addActor(*dropoffZoneBody);
	//----------------------------------------------------------
	//Trigger Shapes--------------------------------------------
	PxShape* pickupShape = instance.gPhysics->createShape(PxBoxGeometry(1.1f, 2.f, 1.1f), *instance.gMaterial, true);
	//trigger box for picking up the flag
	pickupShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pickupShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::pickupBox = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(-10.f, 2.f, -12.f)));
	State::pickupBox->attachShape(*pickupShape);
	instance.gScene->addActor(*State::pickupBox);

	PxShape* dropoffShape = instance.gPhysics->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *instance.gMaterial, true);
	//trigger box for dropping off the flag
	dropoffShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	dropoffShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::dropoffBox = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 1.f, 0.f)));
	State::dropoffBox->attachShape(*dropoffShape);
	instance.gScene->addActor(*State::dropoffBox);
}

void Flag::draw(Physics& instance)
{
	// Draw the flag
	PxTransform transform;
	transform = State::flagBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix);

	// Draw the drop-off box
	transform = State::dropoffBox->getGlobalPose();
	modelMatrix = PxMat44(transform);
	mDropOffZone->draw(modelMatrix);
}

void Flag::simulate(Physics& instance)
{
	// TODO: Assuming that there is only one vehicle, should change that at some point
	if (State::flagPickedUp) {
		if (mVehicles.empty())
			throw std::runtime_error("Error: Must provide at least one vehicle for the flag to collide with");

		PxVec3 pos = mVehicles[0]->getRigidDynamicActor()->getGlobalPose().p;
		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y, pos.z)));
	}
	else {
		State::flagBody->setGlobalPose(PxTransform(PxVec3(-10.f, 2.f, -12.f)));
	}
}

void Flag::cleanUpPhysics()
{
	PX_RELEASE(State::pickupBox);
	PX_RELEASE(State::dropoffBox);
	PX_RELEASE(State::flagBody);
}
