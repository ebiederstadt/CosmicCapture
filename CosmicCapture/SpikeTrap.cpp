#include "SpikeTrap.h"

#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"
#include <fmt/format.h>

SpikeTrap::SpikeTrap(const ShaderProgram& shader, const std::shared_ptr<Camera>& camera): Entity(
	"models/spike_trap.obj", "textures/blank.jpg", shader, camera)
{}

void SpikeTrap::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap)
{
	// Only draw when actually placed
	if (!hasOwningVehicle())
	{
	const PxMat44 modelMatrix(body->getGlobalPose());
		mGeometry->draw(modelMatrix, depthTexture, depth, depthMap);
	}
}

void SpikeTrap::simulate(Physics& instance)
{
	// Waiting for global activation
	// (gives some time for the player that placed the spike trap to move out of the way)
	if (active && !State::spikeTrapActive)
	{
		activationTimer += 1.0f;
		if (activationTimer >= ACTIVATION_TIME)
		{
			State::spikeTrapActive = true;
			activationTimer = 0.0f;
			active = false;
		}
	}

	// Prevent the vehicle from moving while caught in the spike trap
	if (State::spikeTrapInUse && hasAffectedVehicle())
	{
		mAffectedVehicle.value()->setToRestState();
		affectionTimer += 1.0f;
		if (affectionTimer > AFFECTION_TIME)
		{
			State::spikeTrapInUse = false;
			mAffectedVehicle.reset();
			affectionTimer = 0.0f;
			State::spikeTrapFinished = true;
		}
		else
			mAffectedVehicle.value()->setToRestState();
	}
}

void SpikeTrap::cleanUpPhysics()
{
	PX_RELEASE(body);
	PX_RELEASE(State::spikeTrapTriggerBody);
}

void SpikeTrap::processInput(const std::map<MovementFlags, bool>& inputs, Physics& instance)
{
	for (const auto& [key, keyReleased] : inputs)
	{
		if (key == MovementFlags::ACTION && !keyReleased)
		{
			if (hasOwningVehicle())
			{
				PxVec3 currentVehiclePos = mOwnerVehicle.value()->getRigidDynamicActor()->getGlobalPose().p;
				fmt::print("Current vehicle position: ({}, {}, {})\n", currentVehiclePos.x, currentVehiclePos.y, currentVehiclePos.z);
				currentVehiclePos.y -= 2.0f; // Move down slightly

				PxShape* spikeTrapMarker = instance.gPhysics->createShape(PxBoxGeometry(1.0f, 2.0f, 1.0f), *instance.gMaterial, true);
				spikeTrapMarker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
				body = instance.gPhysics->createRigidStatic(PxTransform(currentVehiclePos));
				body->attachShape(*spikeTrapMarker);
				spikeTrapMarker->release();
				instance.gScene->addActor(*body);

				PxShape* speedboostPickupTriggerShape = instance.gPhysics->createShape(PxBoxGeometry(1.1f, 2.f, 1.1f), *instance.gMaterial, true);
				//trigger box for running into the spike trap
				speedboostPickupTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				speedboostPickupTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
				State::spikeTrapTriggerBody = instance.gPhysics->createRigidStatic(PxTransform(currentVehiclePos));
				State::spikeTrapTriggerBody->attachShape(*speedboostPickupTriggerShape);
				instance.gScene->addActor(*State::spikeTrapTriggerBody);

				mOwnerVehicle.reset(); // No longer attached to the vehicle
				State::spikeTrapPickedUp = false; // spike trap has been dropped
				fmt::print("Placed the spike trap!\n");
				active = true;
			}
		}
	}
}
