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



#include "Camera.h"
#include <glm/ext/matrix_transform.hpp>

#include "physx/foundation/PxMat33.h"
#include "Conversions.h"

using namespace physx;

extern float angle;


Camera::Camera(const PxVec3& eye, const PxVec3& dir, float aspect) :
	perspectiveMatrix(glm::perspective(glm::radians(60.0f), aspect, 0.1f, 3000.0f))
{
	mEye = eye;
	mDir = dir.getNormalized();
}

Camera::Camera(float aspect) :
	perspectiveMatrix(glm::perspective(glm::radians(60.0f), aspect, 0.1f, 3000.0f))
{
	mEye = PxVec3(PxZero);
	mDir = PxVec3(PxZero);
}

glm::mat4 Camera::getViewMatrix() const
{
	const auto center = mEye + mDir;

	return lookAt(pxVec2glm(mEye), pxVec2glm(center), { 0.0f, 1.0f, 0.0f });
}

void Camera::updateCamera(const PxMat44& model, float speed, bool isReversing, bool reverseCamera)
{
	const auto modelPos = model.column3.getXYZ();
	auto back = model.column2.getXYZ() * (-7.0f - 0.0005f * speed);
	back.y += 1.0f;
	if (isReversing) { 
		back = model.column2.getXYZ() * (-7.0f - 0.003f * speed);
		back.y += 1.0f;
	}

	if (reverseCamera)
		back = -back;

	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();

	PxQuat qy(angle, viewY);
	back = qy.rotate(back);

	if (lagCounter == 0) {
		
		lagEyes.clear();
		mEye = modelPos + back;
	}

	if (lagCounter > 7) {

		mEye = lagEyes.front();
		lagEyes.erase(lagEyes.begin());
	}

	lagEyes.push_back(modelPos + back);
	lagCounter++;

	mDir = (modelPos - mEye).getNormalized();
}

PxTransform Camera::getTransform() const
{
	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0));

	if (viewY.normalize() < 1e-6f)
		return PxTransform(mEye);

	PxMat33 m(mDir.cross(viewY), viewY, -mDir);
	return PxTransform(mEye, PxQuat(m));
}

PxVec3 Camera::getEye() const
{
	return mEye;
}

PxVec3 Camera::getDir() const
{
	return mDir;
}

