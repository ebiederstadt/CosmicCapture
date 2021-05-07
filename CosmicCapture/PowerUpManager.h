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
	void use(Physics&, const InputInfo&, int);
	void use(Physics&, std::map<MovementFlags, bool>, int);
	void simulate(Physics& instance);
	void draw(Physics&, const ShaderProgram&, const Camera&, bool);
	void cleanUp();

private:
	// Powerup pickup zones
	std::vector<std::unique_ptr<Entity>> mPickupZones;
	
	// Powerups that each player is currently holding
	std::array<std::unique_ptr<Entity>, 4> mHeldPowerUps;
	
	// Powerups that have been deployed
	std::vector<std::unique_ptr<Entity>> mDeployedPowerUps;
};

