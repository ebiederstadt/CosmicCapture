#pragma once

#include <array>
#include <vector>

#include "Entity.h"
#include "Vehicle.h"


class PowerUpManager
{
public:
	PowerUpManager(Physics& instance);
	
	void pickup(Physics&);
	void use(Physics& instance, const InputInfo& inputInfo, int playerNum);
	void simulate(Physics& instance);
	void draw(Physics& instance, const ShaderProgram& texture, const Camera& camera, bool depth);
	void cleanUp();

private:
	// Powerup pickup zones
	std::vector<std::unique_ptr<Entity>> mPickupZones;
	
	// Powerups that each player is currently holding
	std::array<std::unique_ptr<Entity>, 4> mHeldPowerUps;
	
	// Powerups that have been deployed
	std::vector<std::unique_ptr<Entity>> mDeployedPowerUps;
};

