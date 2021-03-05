#include "Vehicle.h"

#include "physics/VehicleCreate.h"
#include "physics/VehicleMovement.h"
#include <physx/vehicle/PxVehicleUtil.h>

Vehicle::Vehicle(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera) :
	Entity("models/carJoined.obj", "textures/blank.jpg", shaderProgram, camera)
{}

void Vehicle::attachPhysics(Physics& instance)
{
	const VehicleDesc vehicleDesc = instance.initVehicleDesc();
	mVehicle4W = createVehicle4W(vehicleDesc, instance.gPhysics, instance.gCooking);
	const PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0),
	                                 PxQuat(PxIdentity));
	mVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	instance.gScene->addActor(*mVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	mVehicle4W->setToRestState();
	mVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	mVehicle4W->mDriveDynData.setUseAutoGears(true);

	instance.gVehicleModeTimer = 0.0f;
	instance.gVehicleOrderProgress = 0;
	VehicleMovement::startBrakeMode();
}

void Vehicle::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap)
{
	std::vector<PxMat44> modelMatrices;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	
	auto* const actor = mVehicle4W->getRigidDynamicActor();
	const PxU32 nbShapes = actor->getNbShapes();
	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	actor->getShapes(shapes, nbShapes);

	for (PxU32 j = 0; j < nbShapes; j++)
	{
		const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actor));
		const PxGeometryHolder h = shapes[j]->getGeometry();

		modelMatrices.push_back(shapePose);
	}

	mGeometry->draw(modelMatrices[6], depthTexture, depth, depthMap);
}

void Vehicle::simulate(Physics& instance)
{
	//Update the control inputs for the vehicle.
	if (VehicleMovement::gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable,
		                                                         VehicleMovement::gVehicleInputData, Physics::timestep, mIsVehicleInAir,
		                                                         *mVehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable,
		                                                        VehicleMovement::gVehicleInputData, Physics::timestep, mIsVehicleInAir,
		                                                        *mVehicle4W);
	}

	//Raycasts.
	PxVehicleWheels* vehicles[1] = {mVehicle4W};
	PxRaycastQueryResult* raycastResults = instance.gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = instance.gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(instance.gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = instance.gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = {{wheelQueryResults, mVehicle4W->mWheelsSimData.getNbWheels()}};
	PxVehicleUpdates(Physics::timestep, grav, *instance.gFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	mIsVehicleInAir = mVehicle4W->getRigidDynamicActor()->isSleeping()
		                  ? false
		                  : PxVehicleIsInAir(vehicleQueryResults[0]);
}

void Vehicle::processInput(const std::map<MovementFlags, bool>& inputs)
{
	for (const auto& [key, keyReleased] : inputs)
	{
		switch (key)
		{
		case MovementFlags::DOWN:
			if (keyReleased)
			{
				if (mInReverseMode)
				{
					VehicleMovement::stopAccelerateForwardsMode();
				}
				else
				{
					VehicleMovement::stopBrakeMode();
				}
			}
			else
			{
				if ((mVehicle4W->mDriveDynData.getEngineRotationSpeed() <= 0) || (mInReverseMode == true))
				{
					//if speed reaches 1 or we are already in reverse mode
					mInReverseMode = true;
					VehicleMovement::stopBrakeMode(); //stop braking
					mVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE); //shift gear for reverse
					VehicleMovement::startAccelerateForwardsMode(); //start reversing
				}
				else
				{
					VehicleMovement::startBrakeMode(); //if speed was not yet 0 start braking
				}
			}
			break;
		case MovementFlags::UP:
			if (keyReleased)
			{
				if (mInReverseMode)
				{
					VehicleMovement::stopBrakeMode();
				}
				else
				{
					VehicleMovement::stopAccelerateForwardsMode();
				}
			}
			else
			{
				mInReverseMode = false;
				mVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST); //shift gear to move forward
				VehicleMovement::startAccelerateForwardsMode(); //start driving forward
			}
			break;
		case MovementFlags::RIGHT:
			keyReleased ? VehicleMovement::stopTurnHardLeftMode() : VehicleMovement::startTurnHardLeftMode();
			break;
		case MovementFlags::LEFT:
			keyReleased ? VehicleMovement::stopTurnHardRightMode() : VehicleMovement::startTurnHardRightMode();
			break;
		}
	}
}

void Vehicle::cleanUpPhysics()
{
	mVehicle4W->getRigidDynamicActor()->release();
	mVehicle4W->free();
}
