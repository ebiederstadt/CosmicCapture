#pragma once

#include "graphics/Geometry.h"
#include "graphics/ShaderProgram.h"
#include "graphics/TextureAPI.h"

#include <physx/PxPhysicsAPI.h>

// Wrapper for all the textures we are using
struct GUITextures
{
	GUITextures()
	{
		TextureAPI* instance = TextureAPI::instance();
		instance->create(blank);
		instance->create(spikeTrapTexture, GL_LINEAR, false);
		instance->create(speedBoostTexture, GL_LINEAR, false);
		instance->create(projectileTexture, GL_LINEAR, false);

		instance->create(compassTexture, GL_LINEAR, false);

		instance->create(logo, GL_LINEAR, false);
		instance->create(font, GL_NEAREST, false);
		instance->create(controls, GL_LINEAR, false);

		instance->create(winScreen, GL_NEAREST, false);
		instance->create(loseScreen, GL_NEAREST, false);

		instance->create(playerSelect, GL_NEAREST, false);
		instance->create(notReady, GL_NEAREST, false);
		instance->create(ready, GL_NEAREST, false);
	}

	TextureAPI::TextureName blank = "textures/blank.png";

	TextureAPI::TextureName spikeTrapTexture = "textures/spike_preview.png";
	TextureAPI::TextureName speedBoostTexture = "textures/speed_boost.png";
	TextureAPI::TextureName projectileTexture = "textures/rocket_preview.png";

	TextureAPI::TextureName compassTexture = "textures/compass.png";

	TextureAPI::TextureName logo = "textures/cosmicLogo.png";
	TextureAPI::TextureName font = "textures/font.bmp";
	TextureAPI::TextureName controls = "textures/controls.png";

	TextureAPI::TextureName winScreen = "textures/winscreen.png";
	TextureAPI::TextureName loseScreen = "textures/losescreen.png";

	TextureAPI::TextureName playerSelect = "textures/playerSelect.png";
	TextureAPI::TextureName notReady = "textures/notReady.png";
	TextureAPI::TextureName ready = "textures/ready.png";
};

struct ScoreDisplay
{
	ScoreDisplay();

	std::array<FontGpuGeometry, 4> scoreDisplays;
	std::array<GUIGPUGeometry, 4> playerDisplays;
	std::array<FontGeometry, 4> scoreGeometry;
	std::array<int, 4> playerScores = {0, 0, 0, 0};

	Texture score = Texture("textures/font.bmp", GL_NEAREST, false);

	// Car Textures
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
	void render(int);
	void renderMenu() const;
	void renderControls() const;
	void renderEndScreen(int) const;

	void renderPlayerSelect(bool, bool ready = false) const;

	void setCompassDirection(const physx::PxMat44&, const physx::PxMat44&);
	void setCompassDirection(const physx::PxMat44&, const physx::PxVec3&);

	static std::array<glm::vec2, 4> generateTexCoordsForNum(unsigned int);

private:
	ShaderProgram mShader;
	ShaderProgram mFontShader;
	GUITextures mTextures;

	TextureAPI* api;

	// Powerups
	GUIGPUGeometry mPowerupDisplay;

	// Compass
	GUIGPUGeometry mCompassDisplay;
	float mCompassAngle{};

	// Main menu
	GUIGPUGeometry mLogoDisplay;
	GUIGPUGeometry mStartDisplay;

	ScoreDisplay mScoreDisplay;

	void renderPowerUpDisplay(unsigned int, int) const;
	void renderCompassDisplay(unsigned int) const;
	void renderScores(unsigned int, int);
};
