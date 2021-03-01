#pragma once

#include <physx/PxPhysicsAPI.h>

using namespace physx;


class VehicleMovement
{
public:
	static void startAccelerateForwardsMode();
	static void startAccelerateReverseMode();
	static void startBrakeMode();
	static void startTurnHardLeftMode();
	static void startTurnHardRightMode();
	static void startHandbrakeTurnLeftMode();
	static void startHandbrakeTurnRightMode();

	static void stopAccelerateForwardsMode();
	static void stopBrakeMode();
	static void stopTurnHardLeftMode();
	static void stopTurnHardRightMode();

	static void releaseAllControls();

	
	static inline PxVehicleDrive4WRawInputData gVehicleInputData;
	constexpr static bool gMimicKeyInputs = true;
};

