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

#pragma once

#include "physx/PxPhysicsAPI.h"


using namespace physx;

enum
{
	DRIVABLE_SURFACE = 0xffff0000,
	UNDRIVABLE_SURFACE = 0x0000ffff
};

void setupDrivableSurface(PxFilterData&);
void setupNonDrivableSurface(PxFilterData&);

PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking(PxFilterData, PxFilterData, const void*, PxU32, PxHitFlags&);

PxQueryHitType::Enum WheelSceneQueryPostFilterBlocking(PxFilterData, PxFilterData, const void*, PxU32, const PxQueryHit&);

PxQueryHitType::Enum WheelSceneQueryPreFilterNonBlocking(PxFilterData, PxFilterData, const void*, PxU32, PxHitFlags&);

PxQueryHitType::Enum WheelSceneQueryPostFilterNonBlocking(PxFilterData, PxFilterData, const void*, PxU32, const PxQueryHit&);


//Data structure for quick setup of scene queries for suspension queries.
class VehicleSceneQueryData
{
public:
	VehicleSceneQueryData();
	~VehicleSceneQueryData() = default;

	//Allocate scene query data for up to maxNumVehicles and up to maxNumWheelsPerVehicle with numVehiclesInBatch per batch query.
	static VehicleSceneQueryData* allocate(PxU32, PxU32, PxU32, PxU32, PxBatchQueryPreFilterShader, PxBatchQueryPostFilterShader, PxAllocatorCallback&);

	//Free allocated buffers.
	void free(PxAllocatorCallback& allocator);

	//Create a PxBatchQuery instance that will be used for a single specified batch.
	static PxBatchQuery* setUpBatchedSceneQuery(PxU32, const VehicleSceneQueryData&, PxScene*);

	//Return an array of scene query results for a single specified batch.
	[[nodiscard]] PxRaycastQueryResult* getRaycastQueryResultBuffer(PxU32) const;

	//Return an array of scene query results for a single specified batch.
	[[nodiscard]] PxSweepQueryResult* getSweepQueryResultBuffer(PxU32) const;

	//Get the number of scene query results that have been allocated for a single batch.
	[[nodiscard]] PxU32 getQueryResultBufferSize() const;

private:

	//Number of queries per batch
	PxU32 mNumQueriesPerBatch;

	//Number of hit results per query
	PxU32 mNumHitResultsPerQuery;

	//One result for each wheel.
	PxRaycastQueryResult* mRaycastResults;
	PxSweepQueryResult* mSweepResults;

	//One hit for each wheel.
	PxRaycastHit* mRaycastHitBuffer;
	PxSweepHit* mSweepHitBuffer;

	//Filter shader used to filter drivable and non-drivable surfaces
	PxBatchQueryPreFilterShader mPreFilterShader;

	//Filter shader used to reject hit shapes that initially overlap sweeps.
	PxBatchQueryPostFilterShader mPostFilterShader;
};
