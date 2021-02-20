#pragma once

#include "physx/PxPhysicsAPI.h"
#include "VehicleSceneQuery.h"

#include "../input.h"

#define PX_RELEASE(x) if(x){x->release();x=NULL;}
#define PVD_HOST "127.0.0.1"

using namespace physx;

struct ActorUserData
{
	ActorUserData()
		: vehicle(nullptr),
		  actor(nullptr)
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
		  chassisMaterial(nullptr),
		  wheelMass(0.0f),
		  wheelWidth(0.0f),
		  wheelRadius(0.0f),
		  wheelMOI(0.0f),
		  wheelMaterial(nullptr),
		  actorUserData(nullptr),
		  shapeUserDatas(nullptr)
	{
	}

	PxF32 chassisMass;
	PxVec3 chassisDims;
	PxVec3 chassisMOI;
	PxVec3 chassisCMOffset;
	PxMaterial* chassisMaterial;
	PxFilterData chassisSimFilterData; //word0 = collide type, word1 = collide against types, word2 = PxPairFlags

	PxF32 wheelMass;
	PxF32 wheelWidth;
	PxF32 wheelRadius;
	PxF32 wheelMOI;
	PxMaterial* wheelMaterial;
	PxU32 numWheels;
	PxFilterData wheelSimFilterData; //word0 = collide type, word1 = collide against types, word2 = PxPairFlags

	ActorUserData* actorUserData;
	ShapeUserData* shapeUserDatas;
};


class Physics
{
public:
	bool inReverseMode;
	static Physics& Instance();
	void Initialize();
	void CleanupPhysics();

	VehicleDesc initVehicleDesc();

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
	void stepPhysics();

	void processInput(const std::map<MovementFlags, bool>& inputs);

	//SnippetVehicle4WCreate
	static void computeWheelCenterActorOffsets4W(PxF32 wheelFrontZ, PxF32 wheelRearZ, const PxVec3& chassisDims,
	                                             PxF32 wheelWidth, PxF32 wheelRadius, PxU32 numWheels,
	                                             PxVec3* wheelCentreOffsets);
	static void setupWheelsSimulationData
	(PxF32 wheelMass, PxF32 wheelMOI, PxF32 wheelRadius, PxF32 wheelWidth,
	 PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
	 const PxVec3& chassisCMOffset, PxF32 chassisMass,
	 PxVehicleWheelsSimData* wheelsSimData);
	static PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehicle4WDesc, PxPhysics* physics, PxCooking* cooking);


	Physics();


	PxDefaultAllocator gAllocator;
	PxDefaultErrorCallback gErrorCallback;

	PxFoundation* gFoundation = nullptr;
	PxPhysics* gPhysics = nullptr;

	PxDefaultCpuDispatcher* gDispatcher = nullptr;
	PxScene* gScene = nullptr;


	PxCooking* gCooking = nullptr;

	PxMaterial* gMaterial = nullptr;

	PxPvd* gPvd = nullptr;

	VehicleSceneQueryData* gVehicleSceneQueryData = nullptr;
	PxBatchQuery* gBatchQuery = nullptr;

	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = nullptr;

	PxRigidStatic* gGroundPlane = nullptr;
	PxVehicleDrive4W* gVehicle4W = nullptr;


	bool gIsVehicleInAir = true;
private:
};
