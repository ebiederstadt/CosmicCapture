#pragma once

#include <array>
#include <vector>

#include "Entity.h"
#include "Vehicle.h"


class PowerUpManager
{
public:
	PowerUpManager(const std::shared_ptr<Camera> camera, Physics& instance);
	
	void pickup(std::shared_ptr<Camera>, Physics&);
	void use(Physics& instance, const std::map<MovementFlags, bool>& inputs, int playerNum);
	void simulate(Physics& instance);
	void draw(Physics& instance, const ShaderProgram& texture, bool depth);
	void cleanUp();

private:
	// Powerup pickup zones
	std::vector<std::unique_ptr<Entity>> mPickupZones;
	
	// Powerups that each player is currently holding
	std::array<std::unique_ptr<Entity>, 4> mHeldPowerUps;
	
	// Powerups that have been deployed
	std::vector<std::unique_ptr<Entity>> mDeployedPowerUps;
};

