//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2019 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#pragma once

#include <physx/PxPhysicsAPI.h>
#include <physx/foundation/PxTransform.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <vector>


class Camera
{
public:
	Camera(const physx::PxVec3&, const physx::PxVec3&, float);
	Camera(float);

	[[nodiscard]] glm::mat4 getViewMatrix() const;
	void updateCamera(const physx::PxMat44&, float, bool, bool);

	void setEye(const physx::PxVec3& vec) { mEye = vec; }

	[[nodiscard]] physx::PxVec3 getEye() const;
	[[nodiscard]] physx::PxVec3 getDir() const;
	[[nodiscard]] physx::PxTransform getTransform() const;

	glm::mat4 perspectiveMatrix;

	int lagCounter = 0;

private:
	std::vector<physx::PxVec3>	lagEyes;
	physx::PxVec3	mEye;
	physx::PxVec3	mDir;
};

