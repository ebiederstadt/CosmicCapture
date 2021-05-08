#include "Flag.h"

#include <memory>
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"


Flag::Flag() :
	Entity("models/flag.obj", "textures/flag_texture.png")
{

	mFlagBody = std::make_unique<Model>("models/flag_body.obj", "textures/flag_body_texture.png");
	lastPos = PxVec3(0.f, 2.f, 0.f);
}

void Flag::attachPhysics(Physics& instance)
{
	PxShape* flag = instance.gPhysics->createShape(PxBoxGeometry(0.1f, 2.f, 0.1f), *instance.gMaterial, true); //create flag shape
	flag->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
	State::flagBody = instance.gPhysics->createRigidDynamic(PxTransform(PxVec3(0.f, 2.f, 0.f)));

	State::flagBody->attachShape(*flag);
	flag->release();
	instance.gScene->addActor(*State::flagBody);

	PxShape* pickupShape = instance.gPhysics->createShape(PxBoxGeometry(4.f, 2.f, 4.f), *instance.gMaterial, true);
	pickupShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pickupShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	State::flagPickupBox = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 2.f, 0.f)));
	State::flagPickupBox->attachShape(*pickupShape);
	instance.gScene->addActor(*State::flagPickupBox);

}

void Flag::draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth)
{
	PxTransform transform = State::flagBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mFlagBody->draw(modelMatrix, shader, camera, depth, 2);
	mGeometry->draw(modelMatrix, shader, camera, depth, 2);
}

void Flag::simulate(Physics& instance)
{
	if (State::resetFlag) {
		State::resetFlag = false;
		resetLastPos();
	}

	if (State::startPickupFlagTimer) {
		
		if (State::flagTimer < 60.f) {
			State::flagTimer += 1.0f;
		}
			
		else {
			State::flagTimer = 0.0f;
			State::startPickupFlagTimer = false;
			State::canPickupFlag = true;
		}
	}

	if (State::flagPickedUpBy[0]) {

		PxVec3 pos = State::vehicles[0]->getRigidDynamicActor()->getGlobalPose().p;
		lastPos = pos;
		PxMat33 rot = PxMat33(State::vehicles[0]->getRigidDynamicActor()->getGlobalPose().q);

		PxVec3 offset = rot * PxVec3(0.0f, 0.0f, 1.0f);

		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x - 1.8f*offset.x, pos.y + 1.8f, pos.z - 1.8f*offset.z)));
	}
	else if (State::flagPickedUpBy[1]) {
		PxVec3 pos = State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p;
		lastPos = pos;
		PxMat33 rot = PxMat33(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().q);

		PxVec3 offset = rot * PxVec3(0.0f, 0.0f, 1.0f);
		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x - 1.8f * offset.x, pos.y + 1.8f, pos.z - 1.8f * offset.z)));
	}
	else if (State::flagPickedUpBy[2]) {
		PxVec3 pos = State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p;
		lastPos = pos;
		PxMat33 rot = PxMat33(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().q);

		PxVec3 offset = rot * PxVec3(0.0f, 0.0f, 1.0f);
		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x - 1.8f * offset.x, pos.y + 1.8f, pos.z - 1.8f * offset.z)));
	}
	else if (State::flagPickedUpBy[3]) {
		PxVec3 pos = State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p;
		lastPos = pos;
		PxMat33 rot = PxMat33(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().q);

		PxVec3 offset = rot * PxVec3(0.0f, 0.0f, 1.0f);
		State::flagBody->setGlobalPose(PxTransform(PxVec3(pos.x - 1.8f * offset.x, pos.y + 1.8f, pos.z - 1.8f * offset.z)));
	}
	else {
		State::flagBody->setGlobalPose(PxTransform(PxVec3(lastPos.x, 2.f, lastPos.z)));
		State::flagPickupBox->setGlobalPose(PxTransform(PxVec3(lastPos.x, 2.f, lastPos.z)));
	}
}

void Flag::resetLastPos() {
	lastPos = PxVec3(0.f, 2.f, 0.f);
	State::flagBody->setGlobalPose(PxTransform(PxVec3(0.f, 2.f, 0.f)));
	State::flagPickupBox->setGlobalPose(PxTransform(PxVec3(0.f, 2.f, 0.f)));
}

void Flag::cleanUpPhysics()
{
	PX_RELEASE(State::flagPickupBox);
	PX_RELEASE(State::flagBody);
}
