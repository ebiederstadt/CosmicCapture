// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2017 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include <new>

#include "physx/PxPhysicsAPI.h"
#include "VehicleCreate.h"
#include "VehicleSceneQuery.h"


using namespace physx;
float peakTorque = 2000.f;
float maxOmega = 2000.f;
float gearSwitchTime = 0.5f;
float clutchStrength = 10.f;

PxRigidStatic* createDrivablePlane(const PxFilterData& simFilterData, PxMaterial* material, PxPhysics* physics)
{
	//Add a plane to the scene.
	PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 0), *material);

	//Get the plane shape so we can set query and simulation filter data.
	PxShape* shapes[1];
	groundPlane->getShapes(shapes, 1);

	//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
	PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	shapes[0]->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	shapes[0]->setSimulationFilterData(simFilterData);

	return groundPlane;
}

static PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking)
{
	// Create descriptor for convex mesh
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = numVerts;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* convexMesh = nullptr;
	PxDefaultMemoryOutputStream buf;
	if (cooking.cookConvexMesh(convexDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		convexMesh = physics.createConvexMesh(id);
	}

	return convexMesh;
}

PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking)
{
	const PxF32 x = dims.x * 0.5f;
	const PxF32 y = dims.y * 0.5f;
	const PxF32 z = dims.z * 0.5f;
	PxVec3 verts[8] =
	{
		PxVec3(x, y, -z),
		PxVec3(x, y, z),
		PxVec3(x, -y, z),
		PxVec3(x, -y, -z),
		PxVec3(-x, y, -z),
		PxVec3(-x, y, z),
		PxVec3(-x, -y, z),
		PxVec3(-x, -y, -z)
	};

	return createConvexMesh(verts, 8, physics, cooking);
}

PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking)
{
	PxVec3 points[2 * 16];
	for (PxU32 i = 0; i < 16; i++)
	{
		const PxF32 cosTheta = PxCos(i * PxPi * 2.0f / 16.0f);
		const PxF32 sinTheta = PxSin(i * PxPi * 2.0f / 16.0f);
		const PxF32 y = radius * cosTheta;
		const PxF32 z = radius * sinTheta;
		points[2 * i + 0] = PxVec3(-width / 2.0f, y, z);
		points[2 * i + 1] = PxVec3(+width / 2.0f, y, z);
	}

	return createConvexMesh(points, 32, physics, cooking);
}

PxRigidDynamic* createVehicleActor
(const PxVehicleChassisData& chassisData,
 PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,
 const PxFilterData& wheelSimFilterData,
 PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes,
 const PxFilterData& chassisSimFilterData,
 PxPhysics& physics)
{
	//We need a rigid body actor for the vehicle.
	//Don't forget to add the actor to the scene after setting up the associated vehicle.
	PxRigidDynamic* vehActor = physics.createRigidDynamic(PxTransform(PxIdentity));

	//Wheel and chassis query filter data.
	//Optional: cars don't drive on other cars.
	PxFilterData wheelQryFilterData;
	setupNonDrivableSurface(wheelQryFilterData);
	PxFilterData chassisQryFilterData;
	setupNonDrivableSurface(chassisQryFilterData);

	//Add all the wheel shapes to the actor.
	for (PxU32 i = 0; i < numWheels; i++)
	{
		PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
		PxShape* wheelShape = PxRigidActorExt::createExclusiveShape(*vehActor, geom, *wheelMaterials[i]);
		wheelShape->setQueryFilterData(wheelQryFilterData);
		wheelShape->setSimulationFilterData(wheelSimFilterData);
		wheelShape->setLocalPose(PxTransform(PxIdentity));
	}

	//Add the chassis shapes to the actor.
	for (PxU32 i = 0; i < numChassisMeshes; i++)
	{
		PxShape* chassisShape = PxRigidActorExt::createExclusiveShape(
			*vehActor, PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);
		chassisShape->setQueryFilterData(chassisQryFilterData);
		chassisShape->setSimulationFilterData(chassisSimFilterData);
		chassisShape->setLocalPose(PxTransform(PxIdentity));
	}

	vehActor->setMass(chassisData.mMass);
	vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
	vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset, PxQuat(PxIdentity)));

	return vehActor;
}

void configureUserData(PxVehicleWheels* vehicle, ActorUserData* actorUserData, ShapeUserData* shapeUserDatas)
{
	if (actorUserData)
	{
		vehicle->getRigidDynamicActor()->userData = actorUserData;
		actorUserData->vehicle = vehicle;
	}

	if (shapeUserDatas)
	{
		PxShape* shapes[PX_MAX_NB_WHEELS + 1];
		vehicle->getRigidDynamicActor()->getShapes(shapes, PX_MAX_NB_WHEELS + 1);
		for (PxU32 i = 0; i < vehicle->mWheelsSimData.getNbWheels(); i++)
		{
			const PxI32 shapeId = vehicle->mWheelsSimData.getWheelShapeMapping(i);
			shapes[shapeId]->userData = &shapeUserDatas[i];
			shapeUserDatas[i].isWheel = true;
			shapeUserDatas[i].wheelId = i;
		}
	}
}

void customizeVehicleToLengthScale(const PxReal lengthScale, PxRigidDynamic* rigidDynamic,
                                   PxVehicleWheelsSimData* wheelsSimData, PxVehicleDriveSimData* driveSimData)
{
	//Rigid body center of mass and moment of inertia.
	{
		PxTransform t = rigidDynamic->getCMassLocalPose();
		t.p *= lengthScale;
		rigidDynamic->setCMassLocalPose(t);

		PxVec3 moi = rigidDynamic->getMassSpaceInertiaTensor();
		moi *= (lengthScale * lengthScale);
		rigidDynamic->setMassSpaceInertiaTensor(moi);
	}

	//Wheels, suspensions, wheel centers, tire/susp force application points.
	{
		for (PxU32 i = 0; i < wheelsSimData->getNbWheels(); i++)
		{
			PxVehicleWheelData wheelData = wheelsSimData->getWheelData(i);
			wheelData.mRadius *= lengthScale;
			wheelData.mWidth *= lengthScale;
			wheelData.mDampingRate *= lengthScale * lengthScale;
			wheelData.mMaxBrakeTorque *= lengthScale * lengthScale;
			wheelData.mMaxHandBrakeTorque *= lengthScale * lengthScale;
			wheelData.mMOI *= lengthScale * lengthScale;
			wheelsSimData->setWheelData(i, wheelData);

			PxVehicleSuspensionData suspData = wheelsSimData->getSuspensionData(i);
			suspData.mMaxCompression *= lengthScale;
			suspData.mMaxDroop *= lengthScale;
			wheelsSimData->setSuspensionData(i, suspData);

			PxVec3 v = wheelsSimData->getWheelCentreOffset(i);
			v *= lengthScale;
			wheelsSimData->setWheelCentreOffset(i, v);

			v = wheelsSimData->getSuspForceAppPointOffset(i);
			v *= lengthScale;
			wheelsSimData->setSuspForceAppPointOffset(i, v);

			v = wheelsSimData->getTireForceAppPointOffset(i);
			v *= lengthScale;
			wheelsSimData->setTireForceAppPointOffset(i, v);
		}
	}

	//Slow forward speed correction.
	{
		wheelsSimData->setSubStepCount(5.0f * lengthScale, 3, 1);
		wheelsSimData->setMinLongSlipDenominator(4.0f * lengthScale);
	}

	//Engine
	if (driveSimData)
	{
		PxVehicleEngineData engineData = driveSimData->getEngineData();
		engineData.mMOI *= lengthScale * lengthScale;
		engineData.mPeakTorque *= lengthScale * lengthScale;
		engineData.mDampingRateFullThrottle *= lengthScale * lengthScale;
		engineData.mDampingRateZeroThrottleClutchEngaged *= lengthScale * lengthScale;
		engineData.mDampingRateZeroThrottleClutchDisengaged *= lengthScale * lengthScale;
		driveSimData->setEngineData(engineData);
	}

	//Clutch.
	if (driveSimData)
	{
		PxVehicleClutchData clutchData = driveSimData->getClutchData();
		clutchData.mStrength *= lengthScale * lengthScale;
		driveSimData->setClutchData(clutchData);
	}

	//Scale the collision meshes too.
	{
		PxShape* shapes[16];
		const PxU32 nbShapes = rigidDynamic->getShapes(shapes, 16);
		for (PxU32 i = 0; i < nbShapes; i++)
		{
			switch (shapes[i]->getGeometryType())
			{
			case PxGeometryType::eSPHERE:
				{
					PxSphereGeometry sphere;
					shapes[i]->getSphereGeometry(sphere);
					sphere.radius *= lengthScale;
					shapes[i]->setGeometry(sphere);
				}
				break;
			case PxGeometryType::ePLANE:
				PX_ASSERT(false);
				break;
			case PxGeometryType::eCAPSULE:
				{
					PxCapsuleGeometry capsule;
					shapes[i]->getCapsuleGeometry(capsule);
					capsule.radius *= lengthScale;
					capsule.halfHeight *= lengthScale;
					shapes[i]->setGeometry(capsule);
				}
				break;
			case PxGeometryType::eBOX:
				{
					PxBoxGeometry box;
					shapes[i]->getBoxGeometry(box);
					box.halfExtents *= lengthScale;
					shapes[i]->setGeometry(box);
				}
				break;
			case PxGeometryType::eCONVEXMESH:
				{
					PxConvexMeshGeometry convexMesh;
					shapes[i]->getConvexMeshGeometry(convexMesh);
					convexMesh.scale.scale *= lengthScale;
					shapes[i]->setGeometry(convexMesh);
				}
				break;
			case PxGeometryType::eTRIANGLEMESH:
				{
					PxTriangleMeshGeometry triMesh;
					shapes[i]->getTriangleMeshGeometry(triMesh);
					triMesh.scale.scale *= lengthScale;
					shapes[i]->setGeometry(triMesh);
				}
				break;
			case PxGeometryType::eHEIGHTFIELD:
				{
					PxHeightFieldGeometry hf;
					shapes[i]->getHeightFieldGeometry(hf);
					hf.columnScale *= lengthScale;
					hf.heightScale *= lengthScale;
					hf.rowScale *= lengthScale;
					shapes[i]->setGeometry(hf);
				}
				break;
			case PxGeometryType::eINVALID:
			case PxGeometryType::eGEOMETRY_COUNT:
				break;
			}
		}
	}
}

void customizeVehicleToLengthScale(const PxReal lengthScale, PxRigidDynamic* rigidDynamic,
                                   PxVehicleWheelsSimData* wheelsSimData, PxVehicleDriveSimData4W* driveSimData)
{
	customizeVehicleToLengthScale(lengthScale, rigidDynamic, wheelsSimData,
	                              static_cast<PxVehicleDriveSimData*>(driveSimData));

	//Ackermann geometry.
	if (driveSimData)
	{
		PxVehicleAckermannGeometryData ackermannData = driveSimData->getAckermannGeometryData();
		ackermannData.mAxleSeparation *= lengthScale;
		ackermannData.mFrontWidth *= lengthScale;
		ackermannData.mRearWidth *= lengthScale;
		driveSimData->setAckermannGeometryData(ackermannData);
	}
}

PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehicle4WDesc, PxPhysics* physics, PxCooking* cooking)
{
	const PxVec3 chassisDims = vehicle4WDesc.chassisDims;
	const PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
	const PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
	const PxU32 numWheels = vehicle4WDesc.numWheels;

	const PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
	const PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

	//Construct a physx actor with shapes for the chassis and wheels.
	//Set the rigid body mass, moment of inertia, and center of mass offset.
	PxRigidDynamic* veh4WActor = nullptr;
	{
		//Construct a convex mesh for a cylindrical wheel.
		PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius, *physics, *cooking);
		//Assume all wheels are identical for simplicity.
		PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
		PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

		//Set the meshes and materials for the driven wheels.
		for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
		}
		//Set the meshes and materials for the non-driven wheels
		for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
		}

		//Chassis just has a single convex shape for simplicity.
		PxConvexMesh* chassisConvexMesh = createChassisMesh(chassisDims, *physics, *cooking);
		PxConvexMesh* chassisConvexMeshes[1] = {chassisConvexMesh};
		PxMaterial* chassisMaterials[1] = {vehicle4WDesc.chassisMaterial};

		//Rigid body data.
		PxVehicleChassisData rigidBodyData;
		rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
		rigidBodyData.mMass = vehicle4WDesc.chassisMass;
		rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

		veh4WActor = createVehicleActor
		(rigidBodyData,
		 wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
		 chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData,
		 *physics);
	}

	//Set up the sim data for the wheels.
	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(numWheels);
	//Compute the wheel center offsets from the origin.
	PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
	const PxF32 frontZ = chassisDims.z * 0.3f;
	const PxF32 rearZ = -chassisDims.z * 0.3f;
	Physics::computeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels,
	                                          wheelCenterActorOffsets);

	//Set up the simulation data for all wheels.
	Physics::setupWheelsSimulationData
	(vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth,
	 numWheels, wheelCenterActorOffsets,
	 vehicle4WDesc.chassisCMOffset, vehicle4WDesc.chassisMass,
	 wheelsSimData);

	//Set up the sim data for the vehicle drive model.
	PxVehicleDriveSimData4W driveSimData;
	{
		//Diff
		PxVehicleDifferential4WData diff;
		diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
		driveSimData.setDiffData(diff);

		//Engine
		PxVehicleEngineData engine;
		engine.mPeakTorque = peakTorque;
		engine.mMaxOmega = maxOmega; 
		driveSimData.setEngineData(engine);

		//Gears
		PxVehicleGearsData gears;
		gears.mSwitchTime = gearSwitchTime;
		driveSimData.setGearsData(gears);

		//Clutch
		PxVehicleClutchData clutch;
		clutch.mStrength = clutchStrength;
		driveSimData.setClutchData(clutch);

		//Ackermann steer accuracy
		PxVehicleAckermannGeometryData ackermann;
		ackermann.mAccuracy = 1.0f;
		ackermann.mAxleSeparation =
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
		ackermann.mFrontWidth =
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
		ackermann.mRearWidth =
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
		driveSimData.setAckermannGeometryData(ackermann);
	}

	//Create a vehicle from the wheels and drive sim data.
	PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(numWheels);
	vehDrive4W->setup(physics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

	//Configure the userdata
	configureUserData(vehDrive4W, vehicle4WDesc.actorUserData, vehicle4WDesc.shapeUserDatas);

	//Free the sim data because we don't need that any more.
	wheelsSimData->free();

	return vehDrive4W;
}
