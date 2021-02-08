#pragma once

#include <ctype.h>
#include "physx/PxPhysicsAPI.h"
#include "VehicleSceneQuery.h"

#define PX_RELEASE(x) if(x){x->release();x=NULL;}
#define PVD_HOST "127.0.0.1"

using namespace physx;

struct ActorUserData
{
	ActorUserData()
		: vehicle(NULL),
		actor(NULL)
	{
	}

	const PxVehicleWheels* vehicle;
	const PxActor* actor;
};

struct ShapeUserData
{
	ShapeUserData()
		: isWheel(false),
		wheelId(0xffffffff)
	{
	}

	bool isWheel;
	PxU32 wheelId;
};

struct VehicleDesc
{
	VehicleDesc()
		: chassisMass(0.0f),
		chassisDims(PxVec3(0.0f, 0.0f, 0.0f)),
		chassisMOI(PxVec3(0.0f, 0.0f, 0.0f)),
		chassisCMOffset(PxVec3(0.0f, 0.0f, 0.0f)),
		chassisMaterial(NULL),
		wheelMass(0.0f),
		wheelWidth(0.0f),
		wheelRadius(0.0f),
		wheelMOI(0.0f),
		wheelMaterial(NULL),
		actorUserData(NULL),
		shapeUserDatas(NULL)
	{
	}

	PxF32 chassisMass;
	PxVec3 chassisDims;
	PxVec3 chassisMOI;
	PxVec3 chassisCMOffset;
	PxMaterial* chassisMaterial;
	PxFilterData chassisSimFilterData;  //word0 = collide type, word1 = collide against types, word2 = PxPairFlags

	PxF32 wheelMass;
	PxF32 wheelWidth;
	PxF32 wheelRadius;
	PxF32 wheelMOI;
	PxMaterial* wheelMaterial;
	PxU32 numWheels;
	PxFilterData wheelSimFilterData;	//word0 = collide type, word1 = collide against types, word2 = PxPairFlags

	ActorUserData* actorUserData;
	ShapeUserData* shapeUserDatas;
};


class Physics {
public:

	static Physics& Instance();
	void Initialize();
	void CleanupPhysics();

	VehicleDesc initVehicleDesc();

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
	void stepPhysics();

	//SnippetVehicle4WCreate
	void computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 numWheels, PxVec3* wheelCentreOffsets);
	void setupWheelsSimulationData
	(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
		const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
		const PxVec3& chassisCMOffset, const PxF32 chassisMass,
		PxVehicleWheelsSimData* wheelsSimData);
	PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehicle4WDesc, PxPhysics* physics, PxCooking* cooking);


	Physics();


	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;

	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;


	PxCooking* gCooking = NULL;

	PxMaterial* gMaterial = NULL;

	PxPvd* gPvd = NULL;

	VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
	PxBatchQuery* gBatchQuery = NULL;

	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

	PxRigidStatic* gGroundPlane = NULL;
	PxVehicleDrive4W* gVehicle4W = NULL;

	bool	gIsVehicleInAir = true;
private:


};