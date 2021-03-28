#include "Flag.h"

#include <memory>
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"
#include "Colors.h"


Flag::Flag(const std::shared_ptr<Camera>& camera) :
	Entity("models/flag.obj", WHITE, camera)
{

	mFlagBody = std::make_unique<Model>("models/flag_body.obj", WHITE, camera);
}

void Flag::attachPhysics(Physics& instance)
{
	PxShape* flag = instance.gPhysics->createShape(PxBoxGeometry(0.1f, 2.f, 0.1f), *instance.gMaterial, true); //create flag shape
	flag->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
	State::flagBody = instance.gPhysics->createRigidDynamic(PxTransform(PxVec3(5.f, 2.f, 5.f)));
	//create static rigid body - wont move
	State::flagBody->attachShape(*flag);
	flag->release();
	instance.gScene->addActor(*State::flagBody);
	//----------------------------------------------------------
	//Trigger Shapes--------------------------------------------
	PxShape* pickupShape = instance.gPhysics->createShape(PxBoxGeometry(10.f, 2.f, 10.f), *instance.gMaterial, true);
	//trigger box for picking up the flag
	pickupShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pickupShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	State::flagPickupBox = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 2.f, 0.f)));
	State::flagPickupBox->attachShape(*pickupShape);
	instance.gScene->addActor(*State::flagPickupBox);

}

void Flag::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth)
{
	// Draw the flag
	PxTransform transform = State::flagBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mFlagBody->draw(modelMatrix, depthTexture, depth, 2);
	mGeometry->draw(modelMatrix, depthTexture, depth, 2);
}

void Flag::simulate(Physics& instance)
{
	float addDeg = 0;
	float rad = 0;

	if (State::flagPickedUpBy[0]) {

		PxVec3 pos = State::vehicles[0]->getRigidDynamicActor()->getGlobalPose().p;
		PxMat33 rot = PxMat33(State::vehicles[0]->getRigidDynamicActor()->getGlobalPose().q);

		PxVec3 offset = rot * PxVec3(0.0f, 0.0f, 1.0f);

		/*
			PxVec3 dir = State::vehicleRDs[0]->getLinearVelocity();
		if (dir.x < 0)
			addDeg = dir.z >= 0 ? 180 : 270;
		else if (dir.z <= 0) addDeg = 360;
		rad = abs(abs(atan(dir.z / dir.x)) - addDeg*M_PI/180);
		*/

		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x - 1.8f*offset.x, pos.y + 1.8f, pos.z - 1.8f*offset.z)));
	}
	else if (State::flagPickedUpBy[1]) {
		PxVec3 pos = State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p;
		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y + 2.0f, pos.z)));
	}
	else if (State::flagPickedUpBy[2]) {
		PxVec3 pos = State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p;
		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y + 2.0f, pos.z)));
	}
	else if (State::flagPickedUpBy[3]) {
		PxVec3 pos = State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p;
		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y + 2.0f, pos.z)));
	}
	else {
		State::flagBody->setGlobalPose(PxTransform(PxVec3(5.f, 2.f, 5.f)));
	}
}

void Flag::cleanUpPhysics()
{
	PX_RELEASE(State::flagPickupBox);
	PX_RELEASE(State::flagBody);
}
