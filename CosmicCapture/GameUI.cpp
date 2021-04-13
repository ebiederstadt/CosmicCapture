#include "GameUI.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/core.h>

#include "GlobalState.h"

using namespace glm;

// extern vec2 g_scale;
// extern vec2 g_pos;

ScoreDisplay::ScoreDisplay()
{
	GUIGeometry quad;

	// Initially, all scores are at zero
	for (int i = 0; i < 4; ++i)
	{
		scoreGeometry[i].texCoords = GameUI::generateTexCoordsForNum(0);
		scoreDisplays[i].uploadData(scoreGeometry[i]);

		playerDisplays[i].uploadData(quad);
	}
}

GameUI::GameUI() :
	mShader("shaders/ui.vert", "shaders/ui.frag"),
	mFontShader("shaders/font.vert", "shaders/font.frag"),
	api(TextureAPI::instance())
{
	mShader.compile();
	mFontShader.compile();
	
	GUIGeometry quad;
	mPowerupDisplay.uploadData(quad);
	mCompassDisplay.uploadData(quad);
	mLogoDisplay.uploadData(quad);
}

void GameUI::render(int playerNum)
{
	unsigned int shaderID = static_cast<unsigned int>(mShader);
	mShader.use();

	renderPowerUpDisplay(shaderID, playerNum);
	renderCompassDisplay(shaderID);
	renderScores(shaderID, playerNum);
}

void GameUI::renderMenu() const
{
	unsigned int shaderID = static_cast<unsigned int>(mShader);
	mShader.use();

	api->bind(mTextures.logo);
	mat4 model(1.0f);
	const auto modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
	mLogoDisplay.drawData();

	Texture::unbind();
}

void GameUI::renderEndScreen() const
{
	mShader.use();

	if (State::scores[0] >= State::winScore)
		api->bind(mTextures.winScreen);
	else
		api->bind(mTextures.loseScreen);

	mat4 model(1.0f);
	const auto modelLoc = glGetUniformLocation(static_cast<unsigned int>(mShader), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

	mLogoDisplay.drawData();

	Texture::unbind();
}

void GameUI::renderPlayerSelect(bool selected, bool ready) const
{
	mShader.use();

	if (!selected)
		api->bind(mTextures.playerSelect);
	else
	{
		if (!ready)
			api->bind(mTextures.notReady);
		else
			api->bind(mTextures.ready);
	}

	mat4 model(1.0f);
	const auto modelLoc = glGetUniformLocation(static_cast<unsigned int>(mShader), "model");
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
	const auto carDirection = carMatrix.column2.getXYZ().getNormalized();

	// Don't car about y axis
	auto target = targetPos;
	target.y = 0.0f;
	carLoc.y = 0.0f;

	const auto targetDirection = (target - carLoc).getNormalized();
	mCompassAngle = atan2(carDirection.z, carDirection.x) - atan2(targetDirection.z, targetDirection.x);
}

void GameUI::renderPowerUpDisplay(unsigned int shaderID, int playerNum) const
{
	if (State::heldPowerUps[0].has_value())
	{
		auto heldPowerup = State::heldPowerUps[playerNum];
		if (heldPowerup.has_value())
		{
			auto value = heldPowerup.value();
			if (value == PowerUpOptions::SPIKE_TRAP)
				api->bind(mTextures.spikeTrapTexture);
			else if (value == PowerUpOptions::SPEED_BOOST)
				api->bind(mTextures.speedBoostTexture);
			else if (value == PowerUpOptions::PROJECTILE)
				api->bind(mTextures.projectileTexture);
		}
	}
	else
		api->bind(mTextures.blank);

	mat4 model = translate(mat4{ 1.0f }, { -0.85f, -0.85f, 0.0f });
	model = scale(model, { 0.25f, 0.25f, 0.0f });

	const auto modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

	mPowerupDisplay.drawData();

	Texture::unbind();
}

void GameUI::renderCompassDisplay(unsigned int shaderID) const
{
	api->bind(mTextures.compassTexture);

	// Decide which direction to rotate the compass
	mat4 model = translate(mat4{ 1.0f }, { 0.0f, 0.79, 0.0f });
	model = scale(model, { 0.19f, 0.19f, 0.0f });

	model = rotate(model, mCompassAngle, { 0.0f, 0.0f, 1.0f });

	const auto modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

	mCompassDisplay.drawData();

	Texture::unbind();
}

void GameUI::renderScores(unsigned int shaderID, int offset)
{
	mFontShader.use();
	api->bind(mTextures.font);

	float yPos = 0.78f;
	constexpr float inc = 0.24f;

	// Helper lambdas
	auto updateScore = [&](const int i)
	{
		if (State::scores[i] != mScoreDisplay.playerScores[i])
		{
			mScoreDisplay.playerScores[i] = State::scores[i];

			// Update the texture coordinates to show the new score
			mScoreDisplay.scoreGeometry[i].texCoords = generateTexCoordsForNum(State::scores[i]);
			mScoreDisplay.scoreDisplays[i].uploadData(mScoreDisplay.scoreGeometry[i]);
		}
	};

	auto draw = [&](const float x, const int i)
	{
		mat4 model = translate(mat4{ 1.f }, { x, yPos, 0.0f });
		model = scale(model, { 0.15f, 0.2f , 0.0f });
		auto modelLoc = glGetUniformLocation(shaderID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

		mScoreDisplay.scoreDisplays[i].drawData();

		// Draw an icon for the player next to their score
		mShader.use();

		model = scale(model, { 0.78f, 0.66f, 0.0f });
		model = translate(model, { -1.62f, 0.3f, 0.f });

		/*model = scale(model, { g_scale.x, g_scale.y, 0.0f });
		model = translate(model, { g_pos.x, g_pos.y, 0.f });*/

		modelLoc = glGetUniformLocation(shaderID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

		mScoreDisplay.carTextures[i].bind();
		mScoreDisplay.playerDisplays[i].drawData();

		api->bind(mTextures.font);
		mFontShader.use();
	};

	// Draw the score for the main player
	updateScore(offset);
	draw(-0.6f, offset);

	// Draw the score for the other players
	for (int i = 1; i < 4; ++i)
	{
		updateScore((i + offset) % 4);
		draw(1.0f, (i + offset) % 4);
		yPos -= inc;
	}

	Texture::unbind();
	mShader.use();
}

std::array<vec2, 4> GameUI::generateTexCoordsForNum(unsigned int num)
{
	// Invalid case: can only handle numbers between 0 and 9
	// In this case just return generic texture coordinates to show the entire image
	if (num > 9)
	{
		return {
			vec2(0.f, 0.f),
			vec2(0.f, 1.f),
			vec2(1.f, 0.f),
			vec2(1.f, 1.f)
		};
	}

	float inc = 1.f / 8.f;

	if (num <= 7)
	{
		return {
			vec2(num * inc, 2.f * inc),
			vec2(num * inc, 3.f * inc),
			vec2((num + 1) * inc, 2.f * inc),
			vec2((num + 1) * inc, 3.f * inc)
		};
	}

	if (num == 8 || num == 9)
	{
		return {
			vec2((num % 8) * inc, 3.f * inc),
			vec2((num % 8) * inc, 4.f * inc),
			vec2(((num % 8) + 1) * inc, 3.f * inc),
			vec2(((num % 8) + 1) * inc, 4.f * inc)
		};
	}
}

