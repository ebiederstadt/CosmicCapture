#pragma once

#include "graphics/Geometry.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"

class GameUI
{
public:
	GameUI();
	void render() const;

private:
	ShaderProgram mShader;

	// Powerups
	GUIGPUGeometry mPowerupDisplay;
	Texture mSpikeTrapTexture;
	Texture mSpeedBoostTexture;
	Texture mProjectileTexture;
};

