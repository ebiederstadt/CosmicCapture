#include "GameUI.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GlobalState.h"

using namespace glm;

GameUI::GameUI() :
	mShader("shaders/ui.vert", "shaders/ui.frag"),
	mSpikeTrapTexture("textures/spike_preview.png", GL_LINEAR, false),
	mSpeedBoostTexture("textures/speed_boost.png", GL_LINEAR, false),
	mProjectileTexture("textures/green_shell.png", GL_LINEAR, false)
{
	mShader.compile();
	
	GUIGeometry quad;

	mPowerupDisplay.uploadData(quad);
}

void GameUI::render() const
{
	unsigned int shaderID = static_cast<unsigned int>(mShader);
	mShader.use();
	
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
		Texture::unbind();

	mat4 model = translate(mat4{ 1.0f }, { -0.85f, -0.85f, 0.0f });
	model = scale(model, { 0.25f, 0.25f, 0.0f });

	const auto modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

	mPowerupDisplay.drawData();

	Texture::unbind();
}
