#pragma once

#include "graphics/Geometry.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"

#include <physx/PxPhysicsAPI.h>

#include "Colors.h"

// Wrapper for all the textures we are using
struct GUITextures
{
	Texture blank = Texture(CLEAR);

	Texture spikeTrapTexture = Texture("textures/spike_preview.png", GL_LINEAR, false);
	Texture speedBoostTexture = Texture("textures/speed_boost.png", GL_LINEAR, false);
	Texture projectileTexture = Texture("textures/rocket_preview.png", GL_LINEAR, false);

	Texture compassTexture = Texture("textures/compass.png", GL_LINEAR, false);

	Texture logo = Texture("textures/cosmicLogo.png", GL_LINEAR, false);
	Texture font = Texture("textures/font.bmp", GL_NEAREST, false);
};

struct ScoreDisplay
{
	ScoreDisplay();
	
	std::array<FontGpuGeometry, 4> scoreDisplays;
	std::array<GUIGPUGeometry, 4> playerDisplays;
	std::array<FontGeometry, 4> scoreGeometry;
	std::array<int, 4> playerScores = { 0, 0, 0, 0 };

	Texture score = Texture("textures/font.bmp", GL_NEAREST, false);

	// Car Textures
	// TODO: Update these to be the actual cars
	std::array<Texture, 4> carTextures = {
		Texture("textures/greenCar.png", GL_LINEAR, false),
		Texture("textures/blueCar.png", GL_LINEAR, false),
		Texture("textures/redCar.png", GL_LINEAR, false),
		Texture("textures/yellowCar.png", GL_LINEAR, false)
	};
};

class GameUI
{
public:
	GameUI();
	void render();
	void renderMenu() const;

	void setCompassDirection(const physx::PxMat44& carMatrix, const physx::PxMat44& targetMatrix);
	void setCompassDirection(const physx::PxMat44& carMatrix, const physx::PxVec3& targetPos);

	static std::array<glm::vec2, 4> generateTexCoordsForNum(unsigned int num);

private:
	ShaderProgram mShader;
	ShaderProgram mFontShader;
	GUITextures mTextures;

	// Powerups
	GUIGPUGeometry mPowerupDisplay;

	// Compass
	GUIGPUGeometry mCompassDisplay;
	float mCompassAngle{};

	// Main menu
	GUIGPUGeometry mLogoDisplay;
	GUIGPUGeometry mStartDisplay;

	ScoreDisplay mScoreDisplay;

	void renderPowerUpDisplay(unsigned int shaderID) const;
	void renderCompassDisplay(unsigned int shaderID) const;
	void renderScores(unsigned int shaderID);
};
