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
#include <ctype.h>
#include <glm/ext/matrix_transform.hpp>

#include "physx/foundation/PxMat33.h"
#include "Conversions.h"

using namespace physx;

extern float angle;


Camera::Camera(const PxVec3& eye, const PxVec3& dir, float aspect) :
	perspectiveMatrix(glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f))
{
	mEye = eye;
	mDir = dir.getNormalized();
	mMouseX = 0;
	mMouseY = 0;
}

void Camera::handleMouse(int button, int state, int x, int y)
{
	PX_UNUSED(state);
	PX_UNUSED(button);
	mMouseX = x;
	mMouseY = y;
}

bool Camera::handleKey(unsigned char key, int x, int y, float speed)
{
	PX_UNUSED(x);
	PX_UNUSED(y);

	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	switch (toupper(key))
	{
	case 'W':	mEye += mDir * 2.0f * speed;		break;
	case 'S':	mEye -= mDir * 2.0f * speed;		break;
	case 'A':	mEye -= viewY * 2.0f * speed;		break;
	case 'D':	mEye += viewY * 2.0f * speed;		break;
	default:							return false;
	}
	return true;
}

void Camera::handleAnalogMove(float x, float y)
{
	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	mEye += mDir * y;
	mEye += viewY * x;
}

glm::mat4 Camera::getViewMatrix() const
{
	const auto center = mEye + mDir;

	return glm::lookAt(pxVec2glm(mEye), pxVec2glm(center), { 0.0f, 1.0f, 0.0f });
}

void Camera::updateCamera(const PxMat44& model, float speed, bool isReversing, bool reverseCamera)
{
	const auto modelPos = model.column3.getXYZ();
	auto back = model.column2.getXYZ() * (-7.0f - 0.005f * speed);
	//auto back = model.column2.getXYZ() * -7.0f;
	back.y += 1.0f;
	if (isReversing) { 
		back = model.column2.getXYZ() * (-7.0f - 0.003f * speed);
		back.y += 1.0f;
		//back.y += 0.001f * speed; 
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

	//mEye = modelPos + back;
	mDir = (modelPos - mEye).getNormalized();
}

void Camera::handleMotion(int x, int y)
{
	int dx = mMouseX - x;
	int dy = mMouseY - y;

	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();

	PxQuat qx(PxPi * dx / 180.0f, PxVec3(0, 1, 0));
	mDir = qx.rotate(mDir);
	PxQuat qy(PxPi * dy / 180.0f, viewY);
	mDir = qy.rotate(mDir);

	mDir.normalize();

	mMouseX = x;
	mMouseY = y;
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

