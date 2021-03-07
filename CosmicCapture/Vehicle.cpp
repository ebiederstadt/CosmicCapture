#include "Vehicle.h"

#include "physics/VehicleCreate.h"
#include "physics/VehicleMovement.h"
#include <physx/vehicle/PxVehicleUtil.h>


Vehicle::Vehicle(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera, int playerNum, std::string texturePath) :
	Entity("models/carJoined.obj", texturePath.c_str(), shaderProgram, camera)
{
	player = playerNum;
	if (player >= 0) {
		movement = VehicleMovement(true);
	}
	else {
		movement = VehicleMovement(false);
	}
	
}

void Vehicle::attachPhysics(Physics& instance)
{
	const VehicleDesc vehicleDesc = instance.initVehicleDesc();
	mVehicle4W = createVehicle4W(vehicleDesc, instance.gPhysics, instance.gCooking);
	float x = 0; 
	float z = 0;
	if (player == 0) {
		x = 100.f;
		z = 100.f;
	}
	else if (player == 1) {
		x = -100.f;
		z = 100.f;
	}
	else if (player == 2) {
		x = 100.f;
		z = -100.f;
	}
	else {
		x = -100.f;
		z = -100.f;
	}
	const PxTransform startTransform(PxVec3(x, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), z),
		PxQuat(PxIdentity)); //inline ternary operators are probably not the best choice but they work for now
	mVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	instance.gScene->addActor(*mVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	mVehicle4W->setToRestState();
	mVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	mVehicle4W->mDriveDynData.setUseAutoGears(true);

	instance.gVehicleModeTimer = 0.0f;
	instance.gVehicleOrderProgress = 0;
	movement.startBrakeMode();
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
	if (movement.gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable,
																movement.gVehicleInputData, Physics::timestep, mIsVehicleInAir,
		                                                         *mVehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable,
			movement.gVehicleInputData, Physics::timestep, mIsVehicleInAir,
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
					movement.stopAccelerateForwardsMode();
				}
				else
				{
					movement.stopBrakeMode();
				}
			}
			else
			{
				if ((mVehicle4W->mDriveDynData.getEngineRotationSpeed() <= 0) || (mInReverseMode == true))
				{
					//if speed reaches 1 or we are already in reverse mode
					mInReverseMode = true;
					movement.stopBrakeMode(); //stop braking
					mVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE); //shift gear for reverse
					movement.startAccelerateForwardsMode(); //start reversing
				}
				else
				{
					movement.startBrakeMode(); //if speed was not yet 0 start braking
				}
			}
			break;
		case MovementFlags::UP:
			if (keyReleased)
			{
				if (mInReverseMode)
				{
					movement.stopBrakeMode();
				}
				else
				{
					movement.stopAccelerateForwardsMode();
				}
			}
			else
			{
				mInReverseMode = false;
				mVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST); //shift gear to move forward
				movement.startAccelerateForwardsMode(); //start driving forward
			}
			break;
		case MovementFlags::RIGHT:
			keyReleased ? movement.stopTurnHardLeftMode() : movement.startTurnHardLeftMode();
			break;
		case MovementFlags::LEFT:
			keyReleased ? movement.stopTurnHardRightMode() : movement.startTurnHardRightMode();
			break;
		}
	}
}

void Vehicle::cleanUpPhysics()
{
	mVehicle4W->getRigidDynamicActor()->release();
	mVehicle4W->free();
}
