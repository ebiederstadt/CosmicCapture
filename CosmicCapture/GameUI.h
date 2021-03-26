#pragma once

#include "graphics/Geometry.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"

#include <physx/PxPhysicsAPI.h>

class GameUI
{
public:
	GameUI();
	void render() const;
	void renderMenu() const;

	void setCompassDirection(const physx::PxMat44& carMatrix, const physx::PxMat44& targetMatrix);
	void setCompassDirection(const physx::PxMat44& carMatrix, const physx::PxVec3& targetPos);

private:
	ShaderProgram mShader;

	// Powerups
	GUIGPUGeometry mPowerupDisplay;
	Texture mSpikeTrapTexture;
	Texture mSpeedBoostTexture;
	Texture mProjectileTexture;

	// Compass
	GUIGPUGeometry mCompassDisplay;
	Texture mCompassTexture;
	float mCompassAngle;

	// Main menu
	GUIGPUGeometry mLogoDisplay;
	GUIGPUGeometry mStartDisplay;
	Texture mLogo;

	void renderPowerUpDisplay(unsigned int shaderID) const;
	void renderCompassDisplay(unsigned int shaderID) const;
};

