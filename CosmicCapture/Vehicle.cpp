#include "Vehicle.h"

#include "physics/VehicleCreate.h"
#include "physics/VehicleMovement.h"
#include <physx/vehicle/PxVehicleUtil.h>
#include "GlobalState.h"

float riseRateAccel = 200.f;
float riseRateBrake = 50.f;
float riseRateHandBrake = 6.f;
float riseRateSteerLeft = 2.5f;
float riseRateSteerRight = 2.5f;
float fallRateAccel = 10.f;
float fallRateBrake = 10.f;
float fallRateHandBrake = 10.f;
float fallRateSteerLeft = 5.f;
float fallRateSteerRight = 5.f;
float gSteerVsForwardSpeedData1A = 0.0f;
float gSteerVsForwardSpeedData1B = 0.75f;
float gSteerVsForwardSpeedData2A = 5.0f;
float gSteerVsForwardSpeedData2B = 0.75f;
float gSteerVsForwardSpeedData3A = 30.0f;
float gSteerVsForwardSpeedData3B = 0.125f;
float gSteerVsForwardSpeedData4A = 120.0f;
float gSteerVsForwardSpeedData4B = 0.1f;


// TODO: Might want to think about a better place to store these definitions (so they are not longer globals)
inline PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		riseRateAccel, //rise rate eANALOG_INPUT_ACCEL
		riseRateBrake, //rise rate eANALOG_INPUT_BRAKE		
		riseRateHandBrake, //rise rate eANALOG_INPUT_HANDBRAKE	
		riseRateSteerLeft, //rise rate eANALOG_INPUT_STEER_LEFT
		riseRateSteerRight, //rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		fallRateAccel, //fall rate eANALOG_INPUT_ACCEL
		fallRateBrake, //fall rate eANALOG_INPUT_BRAKE		
		fallRateHandBrake, //fall rate eANALOG_INPUT_HANDBRAKE	
		fallRateSteerLeft, //fall rate eANALOG_INPUT_STEER_LEFT
		fallRateSteerRight //fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

inline PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		riseRateAccel, //rise rate eANALOG_INPUT_ACCEL
		riseRateBrake, //rise rate eANALOG_INPUT_BRAKE		
		riseRateHandBrake, //rise rate eANALOG_INPUT_HANDBRAKE	
		riseRateSteerLeft, //rise rate eANALOG_INPUT_STEER_LEFT
		riseRateSteerRight, //rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		fallRateAccel, //fall rate eANALOG_INPUT_ACCEL
		fallRateBrake, //fall rate eANALOG_INPUT_BRAKE		
		fallRateHandBrake, //fall rate eANALOG_INPUT_HANDBRAKE	
		fallRateSteerLeft, //fall rate eANALOG_INPUT_STEER_LEFT
		fallRateSteerRight //fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

inline PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	gSteerVsForwardSpeedData1A, gSteerVsForwardSpeedData1B,
	gSteerVsForwardSpeedData2A, gSteerVsForwardSpeedData2B,
	gSteerVsForwardSpeedData3A, gSteerVsForwardSpeedData3B,
	gSteerVsForwardSpeedData4A, gSteerVsForwardSpeedData4B,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
inline PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);
/*
inline PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f, 0.75f,
	5.0f, 0.75f,
	30.0f, 0.125f,
	120.0f, 0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
inline PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);
*/

Vehicle::Vehicle(std::shared_ptr<Camera> camera, int playerNum, std::string modelPath, std::string bodyTexturePath, std::string tireTexturePath) :

	Entity(modelPath.c_str(), bodyTexturePath.c_str(), camera)

{
	player = playerNum;
	if (player >= 0) {
		movement = VehicleMovement(true);
	}
	else {
		movement = VehicleMovement(false);
	}


	//tireTexturePath = "textures/blank.jpg";

	wheel1 = std::make_unique<Model>("models/frontRight.obj", tireTexturePath.c_str(), camera);
	wheel2 = std::make_unique<Model>("models/frontLeft.obj", tireTexturePath.c_str(), camera);
	wheel3 = std::make_unique<Model>("models/backRight.obj", tireTexturePath.c_str(), camera);
	wheel4 = std::make_unique<Model>("models/backLeft.obj", tireTexturePath.c_str(), camera);

}

void Vehicle::attachPhysics(Physics& instance)
{
	const VehicleDesc vehicleDesc = instance.initVehicleDesc();
	mVehicle4W = createVehicle4W(vehicleDesc, instance.gPhysics, instance.gCooking);
	float x = 0; 
	float z = 0;
	if (player == 0) {
		x = 160.f;
		z = 160.f;
	}
	else if (player == 1) {
		x = -160.f;
		z = 160.f;
	}
	else if (player == 2) {
		x = 160.f;
		z = -160.f;
	}
	else {
		x = -160.f;
		z = -160.f;
	}
	const PxTransform startTransform(PxVec3(x, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), z),
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
	movement.startBrakeMode();
}

void Vehicle::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth)
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

	wheel1->draw(modelMatrices[0], depthTexture, depth, 2);
	wheel2->draw(modelMatrices[1], depthTexture, depth, 2);
	wheel3->draw(modelMatrices[2], depthTexture, depth, 2);
	wheel4->draw(modelMatrices[3], depthTexture, depth, 2);
	mGeometry->draw(modelMatrices[4], depthTexture, depth, 2);
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

		case MovementFlags::RESET:
			if (!keyReleased) {
				State::killCars[0] = true;
			}
			break;
		}
	}
}

float Vehicle::getVelocity() {
	float speed = mVehicle4W->mDriveDynData.getEngineRotationSpeed();
	return speed;
}

bool Vehicle::isReversing() {
	return mInReverseMode;
}

void Vehicle::cleanUpPhysics()
{
	mVehicle4W->getRigidDynamicActor()->release();
	mVehicle4W->free();
}
