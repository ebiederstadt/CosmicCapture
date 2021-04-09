#include "ContactReportCallback.h"

#include <fmt/format.h>

#include "../GlobalState.h"
#include "../audio/AudioEngine.h"
#include "Physics.h"


void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		// Flag pickups
		if (pairs[i].triggerActor == State::flagPickupBox && pairs[i].otherActor != State::flagBody && State::canPickupFlag)
		{		
			if (pairs[i].otherActor == State::vehicles[0]->getRigidDynamicActor() && !State::flagPickedUpBy[0] && !State::flagPickedUp) {
				State::flagPickedUpBy[0] = true;
				fmt::print("player 0 picked up flag\n");
				Audio::flag_pickup.playSound();
				
			}
			else if (pairs[i].otherActor == State::vehicles[1]->getRigidDynamicActor() && !State::flagPickedUpBy[1] && !State::flagPickedUp) {
				State::flagPickedUpBy[1] = true;
				fmt::print("player 1 picked up flag\n");
			}
			else if (pairs[i].otherActor == State::vehicles[2]->getRigidDynamicActor() && !State::flagPickedUpBy[2] && !State::flagPickedUp) {
				State::flagPickedUpBy[2] = true;
				fmt::print("player 2 picked up flag\n");
			}
			else if (pairs[i].otherActor == State::vehicles[3]->getRigidDynamicActor() && !State::flagPickedUpBy[3] && !State::flagPickedUp) {
				State::flagPickedUpBy[3] = true;
				fmt::print("player 3 picked up flag\n");
			}
			State::flagPickedUp = true;
		}

		// Flag dropoffs
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[0] && pairs[i].otherActor == State::vehicles[0]->getRigidDynamicActor() && State::flagPickedUpBy[0] && !State::killCars[0])
		{
			fmt::print("player 0 dropped off flag\n");
			State::flagPickedUpBy[0] = false;
			State::flagPickedUp = false;
			State::scores[0]++;
			for (int j = 0; j < 4; j++) {
				State::killCars[j] = true;
			}
			Audio::flag_return.playSound();
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[1] && pairs[i].otherActor == State::vehicles[1]->getRigidDynamicActor() && State::flagPickedUpBy[1])
		{
			fmt::print("player 1 dropped off flag\n");
			State::flagPickedUpBy[1] = false;
			State::flagPickedUp = false;
			State::scores[1]++;
			for (int j = 0; j < 4; j++) {
				State::killCars[j] = true;
			}
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[2] && pairs[i].otherActor == State::vehicles[2]->getRigidDynamicActor() && State::flagPickedUpBy[2])
		{
			fmt::print("player 2 dropped off flag\n");
			State::flagPickedUpBy[2] = false;
			State::flagPickedUp = false;
			State::scores[2]++;
			for (int j = 0; j < 4; j++) {
				State::killCars[j] = true;
			}
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[3] && pairs[i].otherActor == State::vehicles[3]->getRigidDynamicActor() && State::flagPickedUpBy[3])
		{
			fmt::print("player 3 dropped off flag\n");
			State::flagPickedUpBy[3] = false;
			State::flagPickedUp = false;
			State::scores[3]++;
			for (int j = 0; j < 4; j++) {
				State::killCars[j] = true;
			}
		}

		// Powerups
		for (int j = 0; j < 4; ++j)
		{
			if (pairs[i].otherActor == State::vehicles[j]->getRigidDynamicActor() && !State::heldPowerUps[j].has_value())
			{
				for (const auto& [id, geom] : State::projectilePickupTriggerBodies)
				{
					if (pairs[i].triggerActor == geom)
					{
						fmt::print("Player {} picked up projectile.\n", j);
						if(j == 0) Audio::projectile_pickup.playSound();
						State::heldPowerUps[j] = PowerUpOptions::PROJECTILE;
					}
				}
				for (const auto& [id, geom] : State::speedBoostPickupTriggerBodies)
				{
					if (pairs[i].triggerActor == geom)
					{
						fmt::print("Player {} picked up speed boost.\n", j);
						State::heldPowerUps[j] = PowerUpOptions::SPEED_BOOST;
						if (j == 0) Audio::speed_boost_pickup.playSound();
					}
				}

				for (const auto& [id, geom] : State::spikeTrapPickupTriggerBodies)
				{
					if (pairs[i].triggerActor == geom)
					{
						fmt::print("Player {} picked up spike trap\n", j);
						State::heldPowerUps[j] = PowerUpOptions::SPIKE_TRAP;
						if (j == 0) Audio::spike_trap_pickup.playSound();
					}
				}
			}
		}

		//Button switch
		if (pairs[i].triggerActor == State::doorSwitchPickupTriggerBody[0] && !State::arenaSwitch) {
			State::arenaSwitch = true;
		}
		if (pairs[i].triggerActor == State::doorSwitchPickupTriggerBody[1] && !State::arenaSwitch) {
			State::arenaSwitch = true;
		}
		if (pairs[i].triggerActor == State::doorSwitchPickupTriggerBody[2] && !State::arenaSwitch) {
			State::arenaSwitch = true;
		}
		if (pairs[i].triggerActor == State::doorSwitchPickupTriggerBody[3] && !State::arenaSwitch) {
			State::arenaSwitch = true;
		}

		// Handle colliding into the spike trap
		for (auto& [id, spikeTrapState] : State::spike_trap_states)
		{
			if (pairs[i].triggerActor == spikeTrapState.triggerBody && spikeTrapState.active)
			{
				fmt::print("Ran into spike trap!\n");
				spikeTrapState.active = false;
				spikeTrapState.inUse = true;

				// Check to see which player ran into this spiketrap
				for (int j = 0; j < 4; ++j)
				{
					if (pairs[i].otherActor == State::vehicles[j]->getRigidDynamicActor())
					{
						if(j == 0) Audio::collision.playSound();
						spikeTrapState.actingUpon = j;
						break;
					}
				}
			}
		}
	}
}

void ContactReportCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			//car hits
			if ((pairHeader.actors[0] == State::vehicles[1]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[0]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[1]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[0]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[0]) {
					State::killCars[0] = true;
					State::flagPickedUpBy[0] = false;
					State::flagPickedUp = false;
					Audio::flag_lost.playSound();
				}
				else if (State::flagPickedUpBy[1]) {
					State::killCars[1] = true;
					State::flagPickedUpBy[1] = false;
					State::flagPickedUp = false;
				}
				printf("Car 0 and Car 1 have hit\n");
				Audio::car_crash.playSound();
			}
			if ((pairHeader.actors[0] == State::vehicles[2]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[0]->getRigidDynamicActor())|| (pairHeader.actors[1] == State::vehicles[2]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[0]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[0]) {
					State::killCars[0] = true;
					State::flagPickedUpBy[0] = false;
					State::flagPickedUp = false;
					Audio::flag_lost.playSound();
				}
				else if (State::flagPickedUpBy[2]) {
					State::killCars[2] = true;
					State::flagPickedUpBy[2] = false;
					State::flagPickedUp = false;
				}
				printf("Car 0 and Car 2 have hit\n");
				Audio::car_crash.playSound();
			}
			if ((pairHeader.actors[0] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[0]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[0]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[0]) {
					State::killCars[0] = true;
					State::flagPickedUpBy[0] = false;
					State::flagPickedUp = false;
					Audio::flag_lost.playSound();
				}
				else if (State::flagPickedUpBy[3]) {
					State::killCars[3] = true;
					State::flagPickedUpBy[3] = false;
					State::flagPickedUp = false;
				}
				printf("Car 0 and Car 3 have hit\n");
				Audio::car_crash.playSound();
			}
			if ((pairHeader.actors[0] == State::vehicles[2]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[1]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[2]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[1]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[1]) {
					State::killCars[1] = true;
					State::flagPickedUpBy[1] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[2]) {
					State::killCars[2] = true;
					State::flagPickedUpBy[2] = false;
					State::flagPickedUp = false;
				}
				printf("Car 1 and Car 2 have hit\n");
			}
			if ((pairHeader.actors[0] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[1]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[1]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[1]) {
					State::killCars[1] = true;
					State::flagPickedUpBy[1] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[3]) {
					State::killCars[3] = true;
					State::flagPickedUpBy[3] = false;
					State::flagPickedUp = false;
				}
				printf("Car 1 and Car 3 have hit\n");
			}
			if ((pairHeader.actors[0] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[2]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[2]->getRigidDynamicActor())){
				if (State::flagPickedUpBy[2]) {
					State::killCars[2] = true;
					State::flagPickedUpBy[2] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[3]) {
					State::killCars[3] = true;
					State::flagPickedUpBy[3] = false;
					State::flagPickedUp = false;
				}
				printf("Car 2 and Car 3 have hit\n");
			}

			// Handle collisions between vehicles and the projectile
			for (const auto& [id, state] : State::projectileStates)
			{
				// Only check for collisions if the projectile is actually active
				if (!state.active)
					continue;

				for (int j = 0; j < 4; ++j)
				{
					if ((pairHeader.actors[0] == state.body && pairHeader.actors[1] == State::vehicles[j]->getRigidDynamicActor()) ||
						(pairHeader.actors[0] == State::vehicles[j]->getRigidDynamicActor() && pairHeader.actors[1] == state.body))
					{
						fmt::print("Projectile collided with car #{}\n", j);
						State::killCars[j] = true;

						// Note: as of now no bounds for distance from player etc.
						Audio::projectile_explosion.playSound();
						if (State::flagPickedUpBy[j])
						{
							State::flagPickedUpBy[j] = false;
							State::flagPickedUp = false;
						}
					}
				}
			}
		
			//arena hits (only for player)
			if ((pairHeader.actors[0] == State::vehicles[0]->getRigidDynamicActor() && pairHeader.actors[1] == Physics::redDoorBody) || (pairHeader.actors[1] == State::vehicles[0]->getRigidDynamicActor() && pairHeader.actors[0] == Physics::blueDoorBody)) {
				Audio::collision.playSound();
				printf("hit red arena\n");
			}
			if ((pairHeader.actors[0] == State::vehicles[0]->getRigidDynamicActor() && pairHeader.actors[1] == Physics::blueDoorBody) || (pairHeader.actors[1] == State::vehicles[0]->getRigidDynamicActor() && pairHeader.actors[0] == Physics::blueDoorBody)) {
				Audio::collision.playSound();
				printf("hit blue arena\n");
			}
		}
	}
}