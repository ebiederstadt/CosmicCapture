#pragma once

#include <physx/PxPhysicsAPI.h>

using namespace physx;


class VehicleMovement
{
public:
	VehicleMovement() = default;
	VehicleMovement(bool);

	void startAccelerateForwardsMode();
	void startAccelerateReverseMode();
	void startBrakeMode();
	void startTurnHardLeftMode();
	void startTurnHardRightMode();
	void startHandbrakeTurnLeftMode();
	void startHandbrakeTurnRightMode();

	void stopAccelerateForwardsMode();
	void stopBrakeMode();
	void stopTurnHardLeftMode();
	void stopTurnHardRightMode();

	void releaseAllControls();

	
	PxVehicleDrive4WRawInputData gVehicleInputData;
	bool gMimicKeyInputs = true;
};

