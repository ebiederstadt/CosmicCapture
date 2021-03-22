#include "GameUI.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/core.h>

#include "Colors.h"
#include "GlobalState.h"

using namespace glm;

GameUI::GameUI() :
	mShader("shaders/ui.vert", "shaders/ui.frag"),
	mBlank(CLEAR),

	mSpikeTrapTexture("textures/spike_preview.png", GL_LINEAR, false),
	mSpeedBoostTexture("textures/speed_boost.png", GL_LINEAR, false),
	mProjectileTexture("textures/green_shell.png", GL_LINEAR, false),

	mCompassTexture("textures/compass.png", GL_LINEAR, false),

	mLogo("textures/cosmicLogo.png", GL_LINEAR, false)
{
	mShader.compile();
	
	GUIGeometry quad;

	mPowerupDisplay.uploadData(quad);
	mCompassDisplay.uploadData(quad);
	mLogoDisplay.uploadData(quad);
}

void GameUI::render() const
{
	unsigned int shaderID = static_cast<unsigned int>(mShader);
	mShader.use();
	
	renderPowerUpDisplay(shaderID);
	renderCompassDisplay(shaderID);
}

void GameUI::renderMenu() const
{
	unsigned int shaderID = static_cast<unsigned int>(mShader);
	mShader.use();

	mLogo.bind();
	mat4 model(1.0f);
	const auto modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
	mLogoDisplay.drawData();

	Texture::unbind();
}

void GameUI::setCompassDirection(const PxMat44& carMatrix, const PxMat44& targetMatrix)
{
	setCompassDirection(carMatrix, targetMatrix.column3.getXYZ());
}

void GameUI::setCompassDirection(const PxMat44& carMatrix, const PxVec3& targetPos)
{
	auto carLoc = carMatrix.column3.getXYZ();
	// Heading vector for the car
	auto carDirection = carMatrix.column2.getXYZ().getNormalized();

	// Don't car about y axis
	auto target = targetPos;
	target.y = 0.0f;
	carLoc.y = 0.0f;
	carDirection.y = 0.0f;

	const auto targetDirection = (target - carLoc).getNormalized();

	mCompassAngle = acos(targetDirection.dot(carDirection));
}

void GameUI::renderPowerUpDisplay(unsigned int shaderID) const
{
	if (State::heldPowerUps[0].has_value())
	{
		auto value = State::heldPowerUps[0].value();
		if (value == PowerUpOptions::SPIKE_TRAP)
			mSpikeTrapTexture.bind();
		else if (value == PowerUpOptions::SPEED_BOOST)
			mSpeedBoostTexture.bind();
		else if (value == PowerUpOptions::PROJECTILE)
			mProjectileTexture.bind();
	}
	else
		mBlank.bind();

	mat4 model = translate(mat4{ 1.0f }, { -0.85f, -0.85f, 0.0f });
	model = scale(model, { 0.25f, 0.25f, 0.0f });

	const auto modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

	mPowerupDisplay.drawData();

	Texture::unbind();
}

void GameUI::renderCompassDisplay(unsigned int shaderID) const
{
	mCompassTexture.bind();

	// Decide which direction to rotate the compass
	mat4 model = translate(mat4{ 1.0f }, { 0.0f, 0.79, 0.0f });
	model = scale(model, { 0.19f, 0.19f, 0.0f });

	model = rotate(model, mCompassAngle, { 0.0f, 0.0f, 1.0f });

	const auto modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

	mCompassDisplay.drawData();

	Texture::unbind();
}

