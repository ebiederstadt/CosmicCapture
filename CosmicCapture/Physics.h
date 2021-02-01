#pragma once

#include <ctype.h>
#include "physx/PxPhysicsAPI.h"

#define PX_RELEASE(x) if(x){x->release();x=NULL;}
#define PVD_HOST "127.0.0.1"

using namespace physx;


class Physics {
public:

	static Physics& Instance();
	void Initialize();
	void CleanupPhysics();
private:

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

	//VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
	PxBatchQuery* gBatchQuery = NULL;

	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

	PxRigidStatic* gGroundPlane = NULL;
	PxVehicleDrive4W* gVehicle4W = NULL;

	bool					gIsVehicleInAir = true;
};