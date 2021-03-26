#include "Physics.h"

#include <iostream>
#include <physx/vehicle/PxVehicleUtil.h>
#include <physx/PxPhysicsAPI.h>

#include "VehicleTireFriction.h"
#include "VehicleSceneQuery.h"
#include "VehicleCreate.h"
#include "VehicleFilterShader.h"


using namespace physx;


Physics& Physics::Instance()
{
	static Physics instance;
	return instance;
}

//------------------------------------------------------

void Physics::Initialize()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;


	gScene = gPhysics->createScene(sceneDesc);


	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));


	///////////////////////////////////////////////////

	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(4, PX_MAX_NB_WHEELS, 1, 1,
	                                                         WheelSceneQueryPreFilterBlocking, nullptr, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gScene->addActor(*gGroundPlane);

	blueDoorMesh = readMesh("./models/blueArena.obj");

	blueDoorShape = gPhysics->createShape(PxTriangleMeshGeometry(blueDoorMesh), *gMaterial, true); //create shape
	blueDoorShape->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));//set filter data for collisions
	blueDoorBody = gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 0.f, 0.f))); //create static rigid body - wont move
	blueDoorBody->attachShape(*blueDoorShape); //stick shape on rigid body
	blueDoorShape->release(); //free shape 
	gScene->addActor(*blueDoorBody); //add rigid body to scene

	PX_RELEASE(blueDoorBody);

	redDoorMesh = readMesh("./models/redArena.obj");

	redDoorShape = gPhysics->createShape(PxTriangleMeshGeometry(redDoorMesh), *gMaterial, true); //create shape
	redDoorShape->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));//set filter data for collisions
	redDoorBody = gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 0.f, 0.f))); //create static rigid body - wont move
	redDoorBody->attachShape(*redDoorShape); //stick shape on rigid body
	redDoorShape->release(); //free shape 
	gScene->addActor(*redDoorBody); //add rigid body to scene
	//generateRedDoor();

	//----------------------------------------------------------*/

	printf("Physx initialized\n");
}

void Physics::processNodeS(aiNode* node, const aiScene* scene)
{
	// Process all of the the meshes associated with the node
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		auto* const mesh = scene->mMeshes[node->mMeshes[i]];
		processVerticesIndices(mesh);
	}

	// Repeat for all children
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNodeS(node->mChildren[i], scene);
	}
}

void Physics::processVerticesIndices(aiMesh* mesh)
{

	// Process all of the vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		auto vertex = mesh->mVertices[i];
		vectorList.emplace_back(vertex.x, vertex.y, vertex.z);
	}

	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			
			indicesList.push_back(face.mIndices[j]);
		}
	}


}

PxTriangleMeshGeometry Physics::readMesh(std::string modelPath) {
	Assimp::Importer importer;
	const auto* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fmt::print("ERROR::ASSIMP::{}\n", importer.GetErrorString());
		throw std::runtime_error("Failed to load model");
	}
	vectorList.clear();
	indicesList.clear();
	std::cout << vectorList.size() << std::endl;
	std::cout << indicesList.size() << std::endl;

	processNodeS(scene->mRootNode, scene);

	std::cout << vectorList.size() << std::endl;
	std::cout << indicesList.size() << std::endl;

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vectorList.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = reinterpret_cast<const void*>(vectorList.data());

	meshDesc.triangles.count = indicesList.size();
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = reinterpret_cast<const void*>(indicesList.data());
	PxTriangleMesh* triangleMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());

	return triangleMesh;
}

void Physics::generateRedDoor() {
	PX_RELEASE(blueDoorBody);

	redDoorShape = gPhysics->createShape(PxTriangleMeshGeometry(redDoorMesh), *gMaterial, true); //create shape
	redDoorShape->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));//set filter data for collisions
	redDoorBody = gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 0.f, 0.f))); //create static rigid body - wont move
	redDoorBody->attachShape(*redDoorShape); //stick shape on rigid body
	redDoorShape->release(); //free shape 
	gScene->addActor(*redDoorBody); //add rigid body to scene


}

void Physics::generateBlueDoor() {
	PX_RELEASE(redDoorBody);
	
	blueDoorShape = gPhysics->createShape(PxTriangleMeshGeometry(blueDoorMesh), *gMaterial, true); //create shape
	blueDoorShape->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));//set filter data for collisions
	blueDoorBody = gPhysics->createRigidStatic(PxTransform(PxVec3(0.f, 0.f, 0.f))); //create static rigid body - wont move
	blueDoorBody->attachShape(*blueDoorShape); //stick shape on rigid body
	blueDoorShape->release(); //free shape 
	gScene->addActor(*blueDoorBody); //add rigid body to scene
	
}
VehicleDesc Physics::initVehicleDesc() const
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1550.0f;
	const PxVec3 chassisDims(3.2f, 2.0f, 6.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 11.8f,
	 (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 11.8f,
	 (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 11.8f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.15f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 40.0f;
	const PxF32 wheelRadius = 0.8f;
	const PxF32 wheelWidth = 0.5f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

void Physics::stepPhysics() const
{
	gScene->simulate(timestep);
	gScene->fetchResults(true);
}

void Physics::CleanupPhysics()
{
	PX_RELEASE(gGroundPlane);
	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(gAllocator);
	PX_RELEASE(gFrictionPairs);
	PX_RELEASE(redDoorBody);
	PX_RELEASE(blueDoorBody);
	PxCloseVehicleSDK();

	PX_RELEASE(gMaterial);
	PX_RELEASE(gCooking);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();
		gPvd = nullptr;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
	printf("Physx cleaned up\n");
}

//Vehicle Input
//SnippetCreate
void Physics::computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ,
                                               const PxVec3& chassisDims, const PxF32 wheelWidth,
                                               const PxF32 wheelRadius, const PxU32 numWheels,
                                               PxVec3* wheelCentreOffsets)
{
	//chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
	//The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
	//Compute a position for the front wheel and the rear wheel along the z-axis.
	//Compute the separation between each wheel along the z-axis.
	const PxF32 numLeftWheels = numWheels / 2.0f;
	const PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 0.6f);
	//Set the outside of the left and right wheels to be flush with the chassis.
	//Set the top of the wheel to be just touching the underside of the chassis.
	//Begin by setting the rear-left/rear-right/front-left,front-right wheels.
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f,
	                                                                    -(chassisDims.y / 3.5f + wheelRadius),
	                                                                    wheelRearZ + 0 * deltaZ * 0.5f);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = PxVec3(
		(+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 3.5f + wheelRadius), wheelRearZ + 0 * deltaZ * 0.5f);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = PxVec3(
		(-chassisDims.x + wheelWidth + 0.2f) * 0.5f, -(chassisDims.y / 2.6f + wheelRadius),
		wheelRearZ + (numLeftWheels - 1) * deltaZ);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = PxVec3(
		(+chassisDims.x - wheelWidth - 0.2f) * 0.5f, -(chassisDims.y / 2.6f + wheelRadius),
		wheelRearZ + (numLeftWheels - 1) * deltaZ);
	//Set the remaining wheels.
	for (PxU32 i = 2, wheelCount = 4; i < numWheels - 2; i += 2, wheelCount += 2)
	{
		wheelCentreOffsets[wheelCount + 0] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f,
		                                            -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
		wheelCentreOffsets[wheelCount + 1] = PxVec3((+chassisDims.x - wheelWidth) * 0.5f,
		                                            -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
	}
}

void Physics::setupWheelsSimulationData
(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
 const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
 const PxVec3& chassisCMOffset, const PxF32 chassisMass,
 PxVehicleWheelsSimData* wheelsSimData)
{
	//Set up the wheels.
	PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
	{
		//Set up the wheel data structures with mass, moi, radius, width.
		for (PxU32 i = 0; i < numWheels; i++)
		{
			wheels[i].mMass = wheelMass;
			wheels[i].mMOI = wheelMOI;
			wheels[i].mRadius = wheelRadius;
			wheels[i].mWidth = wheelWidth;
		}

		//Enable the handbrake for the rear wheels only.
		wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
		wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
		//Enable steering for the front wheels only.
		wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi * 0.3333f;
		wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi * 0.3333f;
	}

	//Set up the tires.
	PxVehicleTireData tires[PX_MAX_NB_WHEELS];
	{
		//Set up the tires.
		for (PxU32 i = 0; i < numWheels; i++)
		{
			tires[i].mType = TIRE_TYPE_NORMAL;
		}
	}

	//Set up the suspensions
	PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
	{
		//Compute the mass supported by each suspension spring.
		PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
		PxVehicleComputeSprungMasses
		(numWheels, wheelCenterActorOffsets,
		 chassisCMOffset, chassisMass, 1, suspSprungMasses);

		//Set the suspension data.
		for (PxU32 i = 0; i < numWheels; i++)
		{
			suspensions[i].mMaxCompression = 0.3f;
			suspensions[i].mMaxDroop = 0.1f;
			suspensions[i].mSpringStrength = 35000.0f;
			suspensions[i].mSpringDamperRate = 4500.0f;
			suspensions[i].mSprungMass = suspSprungMasses[i];
		}

		//Set the camber angles.
		const PxF32 camberAngleAtRest = 0.0;
		const PxF32 camberAngleAtMaxDroop = 0.01f;
		const PxF32 camberAngleAtMaxCompression = -0.01f;
		for (PxU32 i = 0; i < numWheels; i += 2)
		{
			suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
			suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
			suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
			suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
			suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
			suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
		}
	}

	//Set up the wheel geometry.
	PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
	PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
	PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
	PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
	{
		//Set the geometry data.
		for (PxU32 i = 0; i < numWheels; i++)
		{
			//Vertical suspension travel.
			suspTravelDirections[i] = PxVec3(0, -1, 0);

			//Wheel center offset is offset from rigid body center of mass.
			wheelCentreCMOffsets[i] =
				wheelCenterActorOffsets[i] - chassisCMOffset;

			//Suspension force application point 0.3 metres below 
			//rigid body center of mass.
			suspForceAppCMOffsets[i] =
				PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

			//Tire force application point 0.3 metres below 
			//rigid body center of mass.
			tireForceAppCMOffsets[i] =
				PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
		}
	}

	//Set up the filter data of the raycast that will be issued by each suspension.
	PxFilterData qryFilterData;
	setupNonDrivableSurface(qryFilterData);

	//Set the wheel, tire and suspension data.
	//Set the geometry data.
	//Set the query filter data
	for (PxU32 i = 0; i < numWheels; i++)
	{
		wheelsSimData->setWheelData(i, wheels[i]);
		wheelsSimData->setTireData(i, tires[i]);
		wheelsSimData->setSuspensionData(i, suspensions[i]);
		wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
		wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
		wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
		wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
		wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
		wheelsSimData->setWheelShapeMapping(i, static_cast<PxI32>(i));
	}

	//Add a front and rear anti-roll bar
	PxVehicleAntiRollBarData barFront;
	barFront.mWheel0 = PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
	barFront.mWheel1 = PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
	barFront.mStiffness = 10000.0f;
	wheelsSimData->addAntiRollBarData(barFront);
	PxVehicleAntiRollBarData barRear;
	barRear.mWheel0 = PxVehicleDrive4WWheelOrder::eREAR_LEFT;
	barRear.mWheel1 = PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
	barRear.mStiffness = 10000.0f;
	wheelsSimData->addAntiRollBarData(barRear);
}

PxTriangleMesh* Physics::createTriangleMesh32(PxPhysics* physics, PxCooking* cooking, const PxVec3* verts,
                                              PxU32 vertCount, const PxU32* indices32, PxU32 triCount, bool insert)
{
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vertCount;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts;

	meshDesc.triangles.count = triCount;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices32;

	if (!insert)
	{
		PxDefaultMemoryOutputStream writeBuffer;
		bool status = cooking->cookTriangleMesh(meshDesc, writeBuffer);
		if (!status)
			return nullptr;

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		return physics->createTriangleMesh(readBuffer);
	}

	return cooking->createTriangleMesh(meshDesc, physics->getPhysicsInsertionCallback());
}
