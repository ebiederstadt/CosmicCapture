#include "PowerUpManager.h"

#include "GlobalState.h"
#include "SpikeTrap.h"
#include "SpeedBoost.h"
#include "SpeedBoostPickupZone.h"
#include "SpikeTrapPickupZone.h"
#include "ProjectilePickupZone.h"
#include "Projectile.h"

#include "./audio/AudioEngine.h"

PowerUpManager::PowerUpManager(Physics& instance)
{

	mPickupZones.push_back(std::make_unique<SpeedBoostPickupZone>(PxVec3(0.f, 0.f, 560.f / 2)));
	mPickupZones[0]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<SpeedBoostPickupZone>(PxVec3(0.0f, 0.f, -560.f / 2)));
	mPickupZones[1]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<SpeedBoostPickupZone>( PxVec3(560.0f / 2, 0.f, 0.f)));
	mPickupZones[2]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<SpeedBoostPickupZone>(PxVec3(-560.0f / 2, 0.f, 0.f)));
	mPickupZones[3]->attachPhysics(instance);

	mPickupZones.push_back(std::make_unique<SpikeTrapPickupZone>(PxVec3(0.f, 0.f, 180.f / 2)));
	mPickupZones[4]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<SpikeTrapPickupZone>(PxVec3(0.f, 0.f, -180.f / 2)));
	mPickupZones[5]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<SpikeTrapPickupZone>(PxVec3(180.f / 2, 0.f, 0.f)));
	mPickupZones[6]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<SpikeTrapPickupZone>(PxVec3(-180.f / 2, 0.f, 0.f)));
	mPickupZones[7]->attachPhysics(instance);
	
	mPickupZones.push_back(std::make_unique<ProjectilePickupZone>(PxVec3(400.f / 2, 0.f, 400.f / 2)));
	mPickupZones[8]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<ProjectilePickupZone>(PxVec3(400.f / 2, 0.f, -400.f / 2)));
	mPickupZones[9]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<ProjectilePickupZone>(PxVec3(-400.f / 2, 0.f, 400.f / 2)));
	mPickupZones[10]->attachPhysics(instance);
	mPickupZones.push_back(std::make_unique<ProjectilePickupZone>(PxVec3(-400.f / 2, 0.f, -400.f / 2)));
	mPickupZones[11]->attachPhysics(instance);

	
}

void PowerUpManager::pickup(Physics& instance)
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
					mHeldPowerUps[index] = std::make_unique<Projectile>();
					dynamic_cast<Projectile*>(mHeldPowerUps[index].get())->attachVehicle(State::vehicles[index]);
				} else if (iter->value() == PowerUpOptions::SPEED_BOOST)
				{
					mHeldPowerUps[index] = std::make_unique<SpeedBoost>();
					mHeldPowerUps[index]->attachPhysics(instance);
					dynamic_cast<SpeedBoost*>(mHeldPowerUps[index].get())->attachVehicle(State::vehicles[index]);
				} else if (iter->value() == PowerUpOptions::SPIKE_TRAP)
				{
					mHeldPowerUps[index] = std::make_unique<SpikeTrap>();
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

void PowerUpManager::use(Physics& instance, const InputInfo& inputInfo, int playerNum)
{
	// TODO: We don't have access to which player the powerup is acting on here
	// As a result, everybody will use the powerup at the same time, even if they did not
	// actually want to use the powerup
	if (playerNum < 0 || playerNum > 4)
	{
		fmt::print("WARNING: Attempting to use powerups on a player that does not exist");
		return;
	}

	if (dynamic_cast<Projectile*>(mHeldPowerUps[playerNum].get()))
	{
		if (!inputInfo.inputState.at(MovementFlags::ACTION))
		{
			fmt::print("Using Projectile\n");
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			powerup->attachPhysics(instance);
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();

			if (playerNum == 0) Audio::projectile.playSound();
		}
	} else if (dynamic_cast<SpeedBoost*>(mHeldPowerUps[playerNum].get()))
	{
		// Use the powerup when the player presses the use key
		if (!inputInfo.inputState.at(MovementFlags::ACTION))
		{
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();

			if (playerNum == 0) Audio::speed_boost.playSound();
		}
	} else if (dynamic_cast<SpikeTrap*>(mHeldPowerUps[playerNum].get()))
	{
		// Check to see if the spike trap should be placed, and move it into the deployed powerups list if it should
		if (dynamic_cast<SpikeTrap*>(mHeldPowerUps[playerNum].get())->processInput(inputInfo.inputState, instance))
		{
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();

			if (playerNum == 0) Audio::collision.playSound();
		}
	}
}
void PowerUpManager::use(Physics& instance, std::map<MovementFlags, bool> inputState, int playerNum)
{
	// TODO: We don't have access to which player the powerup is acting on here
	// As a result, everybody will use the powerup at the same time, even if they did not
	// actually want to use the powerup
	if (playerNum < 0 || playerNum > 4)
	{
		fmt::print("WARNING: Attempting to use powerups on a player that does not exist");
		return;
	}

	if (dynamic_cast<Projectile*>(mHeldPowerUps[playerNum].get()))
	{
		if (!inputState.at(MovementFlags::ACTION))
		{
			fmt::print("Using Projectile\n");
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			powerup->attachPhysics(instance);
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();

			if (playerNum == 0) Audio::projectile.playSound();
		}
	}
	else if (dynamic_cast<SpeedBoost*>(mHeldPowerUps[playerNum].get()))
	{
		// Use the powerup when the player presses the use key
		if (!inputState.at(MovementFlags::ACTION))
		{
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();

			if (playerNum == 0) Audio::speed_boost.playSound();
		}
	}
	else if (dynamic_cast<SpikeTrap*>(mHeldPowerUps[playerNum].get()))
	{
		// Check to see if the spike trap should be placed, and move it into the deployed powerups list if it should
		if (dynamic_cast<SpikeTrap*>(mHeldPowerUps[playerNum].get())->processInput(inputState, instance))
		{
			auto powerup = static_cast<std::unique_ptr<Entity>>(mHeldPowerUps[playerNum].release());
			mDeployedPowerUps.push_back(std::move(powerup));
			State::heldPowerUps[playerNum].reset();

			if (playerNum == 0) Audio::collision.playSound();
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
			int id = spikeTrap->getID();
			try {
				auto state = State::spike_trap_states.at(id);
				// Catch a player with the spike trap:
				if (state.inUse && ! spikeTrap->hasAffectedVehicle())
				{
					fmt::print("Player {} ran into spike trap\n", state.actingUpon);
					spikeTrap->attachAffectedVehicle(State::vehicles[state.actingUpon]);
				}

				// Remove the spike trap once it is placed and trapped somebody
				if (spikeTrap->shouldBeDeleted)
				{
					spikeTrap->cleanUpPhysics();
					powerup = mDeployedPowerUps.erase(powerup);

					increment = false; // Don't increment when we erase from the vector
				}
			} catch (std::out_of_range&)
			{
				fmt::print("WARNING: Attempting to operate on an invalid spike trap: {}\n", id);
			}
		}

		if (powerup != mDeployedPowerUps.end())
		{

			// Speed boost specific stuff
			const auto speedBoost = dynamic_cast<SpeedBoost*>(powerup->get());
			if (speedBoost)
			{
				if (speedBoost->shouldBeDeleted)
				{
					speedBoost->cleanUpPhysics();
					powerup = mDeployedPowerUps.erase(powerup);
					continue;
				}
			}

			// Projectile specific stuff
			const auto projectile = dynamic_cast<Projectile*>(powerup->get());
			if (projectile)
			{
				if (projectile->shouldBeDeleted)
				{
					projectile->cleanUpPhysics();
					powerup = mDeployedPowerUps.erase(powerup);
					continue;
				}
			}
		}

		// Only increment when we did not erase anything from deployed powerups
		if (increment)
			++powerup;
	}
}

void PowerUpManager::draw(Physics& instance, const ShaderProgram& texture, const Camera& camera, bool depth)
{
	// Draw pickup zones
	for (const auto& pickupZone : mPickupZones)
	{
		pickupZone->draw(instance, texture, camera, depth);
	}
	
	// Draw powerups
	for (const auto& powerup : mDeployedPowerUps)
	{
		powerup->draw(instance, texture, camera, depth);
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
