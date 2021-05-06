#pragma once

#include <assimp/Importer.hpp>
#include <fmt/core.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <vector>

#include "ContactReportCallback.h"
#include "physx/PxPhysicsAPI.h"
#include "VehicleSceneQuery.h"

template <typename T>
constexpr void PX_RELEASE(T& x)
{
	if (x)
	{
		x->release();
		x = nullptr;
	}
}

constexpr char PVD_HOST[] = "127.0.0.1";

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

extern float chassMass;
extern float chassDimX;
extern float chassDimY;
extern float chassDimZ;
extern float whMass;
extern float whRadius;
extern float whWidth;
extern float springMaxCompression;
extern float springMaxDroop;
extern float springStrength;
extern float springDamperRate;

class Physics
{
public:
	bool inReverseMode;
	static Physics& Instance();
	void Initialize();
	void CleanupPhysics();

	void generateRedDoor();
	void generateBlueDoor();

	PxTriangleMeshGeometry redDoorMesh = nullptr;
	PxTriangleMeshGeometry blueDoorMesh = nullptr;
	PxShape* blueDoorShape = nullptr;
	inline static PxRigidStatic* blueDoorBody = nullptr;
	PxShape* redDoorShape = nullptr;
	inline static PxRigidStatic* redDoorBody = nullptr;

	[[nodiscard]] VehicleDesc initVehicleDesc() const;

	void stepPhysics() const;

	PxTriangleMesh* readMesh(std::string modelPath);

	void processNodeS(aiNode* node, const aiScene* scene);
	void processVerticesIndices(aiMesh* mesh);

	//SnippetVehicle4WCreate
	static void computeWheelCenterActorOffsets4W(PxF32 wheelFrontZ, PxF32 wheelRearZ, const PxVec3& chassisDims,
	                                             PxF32 wheelWidth, PxF32 wheelRadius, PxU32 numWheels,
	                                             PxVec3* wheelCentreOffsets);
	static void setupWheelsSimulationData
	(PxF32 wheelMass, PxF32 wheelMOI, PxF32 wheelRadius, PxF32 wheelWidth,
	 PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
	 const PxVec3& chassisCMOffset, PxF32 chassisMass,
	 PxVehicleWheelsSimData* wheelsSimData);

	// Singleton
	Physics() {}

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

	PxF32 gVehicleModeLifetime = 4.0f;
	PxF32 gVehicleModeTimer = 0.0f;
	PxU32 gVehicleOrderProgress = 0;
	bool gVehicleOrderComplete = false;

	ContactReportCallback gContactReportCallback{};

	static constexpr PxF32 timestep = 1.0f / 60.0f;

private:
	// Cooking parameters
	std::vector<PxVec3> vectorList;
	std::vector<unsigned int> indicesList;

	static PxTriangleMesh* createTriangleMesh32(PxPhysics* physics, PxCooking* cooking, const PxVec3* verts,
	                                            PxU32 vertCount, const PxU32* indices32, PxU32 triCount, bool insert);
};
