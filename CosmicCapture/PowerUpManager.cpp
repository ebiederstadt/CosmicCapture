#include "PowerUpManager.h"

#include "GlobalState.h"
#include "SpikeTrap.h"
#include "SpeedBoost.h"
#include "SpeedBoostPickupZone.h"
#include "SpikeTrapPickupZone.h"
#include "ProjectilePickupZone.h"
#include "Projectile.h"

PowerUpManager::PowerUpManager(const std::shared_ptr<Camera> camera, Physics& instance)
{
	mPickupZones.push_back(std::make_unique<SpeedBoostPickupZone>(camera));
	mPickupZones[0]->attachPhysics(instance);

	mPickupZones.push_back(std::make_unique<SpikeTrapPickupZone>(camera));
	mPickupZones[1]->attachPhysics(instance);
	
	mPickupZones.push_back(std::make_unique<ProjectilePickupZone>(camera));
	mPickupZones[2]->attachPhysics(instance);
}

void PowerUpManager::pickup(const std::shared_ptr<Camera> camera, Physics& instance)
{
	for (auto iter = State::heldPowerUps.begin(); iter < State::heldPowerUps.end(); ++iter)
	{
		auto index = std::distance(State::heldPowerUps.begin(), iter);

		if (iter->has_value())
		{
			// Check to see if we have already added the powerup to the game world
			if (mHeldPowerUps[index] == nullptr)
			{
				if (iter->value() == PowerUpOptions::PROJECTILE)
				{
					mHeldPowerUps[index] = std::make_unique<Projectile>(camera);
					dynamic_cast<Projectile*>(mHeldPowerUps[index].get())->attachVehicle(State::vehicles[index]);
				} else if (iter->value() == PowerUpOptions::SPEED_BOOST)
				{
					mHeldPowerUps[index] = std::make_unique<SpeedBoost>(camera);
					mHeldPowerUps[index]->attachPhysics(instance);
					dynamic_cast<SpeedBoost*>(mHeldPowerUps[index].get())->attachVehicle(State::vehicles[index]);
				} else if (iter->value() == PowerUpOptions::SPIKE_TRAP)
				{
					mHeldPowerUps[index] = std::make_unique<SpikeTrap>(camera);
					dynamic_cast<SpikeTrap*>(mHeldPowerUps[index].get())->attachOwningVehicle(State::vehicles[index]);
					mHeldPowerUps[index]->attachPhysics(instance);
				} else
				{
					// Spam this warning to the console so that people remember to add stuff here if more powerups are added
					fmt::print("WARNING: Attempting to use powerup not handled in PowerUpManager::pickup.\n");
				}
			}
		}
	}
}

void PowerUpManager::use(Physics& instance, const std::map<MovementFlags, bool>& inputs, int playerNum)
{
	if (playerNum < 0 || playerNum > 4)
	{
		fmt::print("WARNING: Attempting to use powerups on a player that does not exist");
		return;
	}

	if (dynamic_cast<Projectile*>(mHeldPowerUps[playerNum].get()))
	{
		if (!inputs.at(MovementFlags::ACTION))
		{
			fmt::print("Using Projectile\n");
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			powerup->attachPhysics(instance);
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();
		}
	} else if (dynamic_cast<SpeedBoost*>(mHeldPowerUps[playerNum].get()))
	{
		// Use the powerup when the player presses the use key
		if (!inputs.at(MovementFlags::ACTION))
		{
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();
		}
	} else if (dynamic_cast<SpikeTrap*>(mHeldPowerUps[playerNum].get()))
	{
		// Check to see if the spike trap should be placed, and move it into the deployed powerups list if it should
		if (dynamic_cast<SpikeTrap*>(mHeldPowerUps[playerNum].get())->processInput(inputs, instance))
		{
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();
		}
	}
}

void PowerUpManager::simulate(Physics& instance)
{
	for (const auto& pickupZone : mPickupZones)
	{
		pickupZone->simulate(instance);
	}

	auto powerup = mDeployedPowerUps.begin();
	while (powerup != mDeployedPowerUps.end())
	{
		powerup->get()->simulate(instance);

		// Spike trap specific stuff
		bool increment = true;
		auto* spikeTrap = dynamic_cast<SpikeTrap*>(powerup->get());
		if (spikeTrap)
		{
			auto state = State::spike_trap_states.begin();
			while (state != State::spike_trap_states.end())
			{
				// Catch a player with the spike trap:
				if (state->second.inUse && ! spikeTrap->hasAffectedVehicle())
				{
					fmt::print("Player {} ran into spike trap\n", state->second.actingUpon);
					spikeTrap->attachAffectedVehicle(State::vehicles[state->second.actingUpon]);
				}

				// Remove the spike trap once it is placed and trapped somebody
				if (state->second.finished)
				{
					powerup->get()->cleanUpPhysics();
					state = State::spike_trap_states.erase(state);
					powerup = mDeployedPowerUps.erase(powerup);

					increment = false; // Don't increment when we erase from the vector
				}
				else
					++state;
			}
		}

		// Speed boost specific stuff
		if (powerup != mDeployedPowerUps.end())
		{
			const auto speedBoost = dynamic_cast<SpeedBoost*>(powerup->get());
			if (speedBoost)
			{
				if (State::speedBoostFinished)
				{
					powerup->get()->cleanUpPhysics();
					State::speedBoostFinished = false;
					powerup = mDeployedPowerUps.erase(powerup);

					increment = false;
				}
			}

			if (increment)
				++powerup;
		}
	}
}

void PowerUpManager::draw(Physics& instance, const ShaderProgram& texture, bool depth)
{
	// Draw pickup zones
	for (const auto& pickupZone : mPickupZones)
	{
		pickupZone->draw(instance, texture, depth);
	}
	
	// Draw powerups
	for (const auto& powerup : mDeployedPowerUps)
	{
		powerup->draw(instance, texture, depth);
	}
}

void PowerUpManager::cleanUp()
{
	for (const auto& pickupZone : mPickupZones)
	{
		pickupZone->cleanUpPhysics();
	}

	for (const auto& heldPowerup : mHeldPowerUps)
	{
		if (heldPowerup != nullptr)
			heldPowerup->cleanUpPhysics();
	}

	for (const auto& deployedPowerup : mDeployedPowerUps)
	{
		deployedPowerup->cleanUpPhysics();
	}

	mPickupZones.clear();
	mDeployedPowerUps.clear();
	for (int i = 0; i < 4; ++i)
		mHeldPowerUps[i].reset();
}
