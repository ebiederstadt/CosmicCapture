#include "ContactReportCallback.h"

#include <fmt/format.h>
#include "../GlobalState.h"
#include <iostream>

#include "../audio/AudioEngine.h"

AudioEngine soundSystem2 = AudioEngine();

AudioInstance flag_pickup = soundSystem2.createInstance(audioConstants::SOUND_FILE_FLAG_PICKUP);
AudioInstance projectile_pickup = soundSystem2.createInstance(audioConstants::SOUND_FILE_PROJECTILE_PICKUP);
AudioInstance spike_trap_pickup = soundSystem2.createInstance(audioConstants::SOUND_FILE_SPIKE_TRAP_PICKUP);
AudioInstance speed_boost_pickup = soundSystem2.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST_PICKUP);
AudioInstance flag_return = soundSystem2.createInstance(audioConstants::SOUND_FILE_FLAG_RETURN);

void ContactReportCallback::initializeSoundTriggers() {

	soundSystem2.initialize();
	soundSystem2.initializeBuffers();

	flag_pickup = soundSystem2.createInstance(audioConstants::SOUND_FILE_FLAG_PICKUP);
	projectile_pickup = soundSystem2.createInstance(audioConstants::SOUND_FILE_PROJECTILE_PICKUP);
	spike_trap_pickup = soundSystem2.createInstance(audioConstants::SOUND_FILE_SPIKE_TRAP_PICKUP);
	speed_boost_pickup = soundSystem2.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST_PICKUP);
	flag_return = soundSystem2.createInstance(audioConstants::SOUND_FILE_FLAG_RETURN);
}

void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		// Flag pickups
		if (pairs[i].triggerActor == State::flagPickupBox && pairs[i].otherActor != State::flagBody)
		{		
			if (pairs[i].otherActor == State::vehicles[0]->getRigidDynamicActor() && !State::flagPickedUpBy[0] && !State::flagPickedUp) {
				State::flagPickedUpBy[0] = true;
				fmt::print("player 0 picked up flag\n");
				flag_pickup.playSound();
				
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
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[0] && pairs[i].otherActor == State::vehicles[0]->getRigidDynamicActor() && State::flagPickedUpBy[0])
		{
			fmt::print("player 0 dropped off flag\n");
			State::flagPickedUpBy[0] = false;
			State::flagPickedUp = false;
			State::scores[0]++;
			flag_return.playSound();
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[1] && pairs[i].otherActor == State::vehicles[1]->getRigidDynamicActor() && State::flagPickedUpBy[1])
		{
			fmt::print("player 1 dropped off flag\n");
			State::flagPickedUpBy[1] = false;
			State::flagPickedUp = false;
			State::scores[1]++;
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[2] && pairs[i].otherActor == State::vehicles[2]->getRigidDynamicActor() && State::flagPickedUpBy[2])
		{
			fmt::print("player 2 dropped off flag\n");
			State::flagPickedUpBy[2] = false;
			State::flagPickedUp = false;
			State::scores[2]++;
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[3] && pairs[i].otherActor == State::vehicles[3]->getRigidDynamicActor() && State::flagPickedUpBy[3])
		{
			fmt::print("player 3 dropped off flag\n");
			State::flagPickedUpBy[3] = false;
			State::flagPickedUp = false;
			State::scores[3]++;
		}

		// Powerups
		for (int j = 0; j < 4; ++j)
		{
			if (pairs[i].otherActor == State::vehicles[j]->getRigidDynamicActor() && !State::heldPowerUps[j].has_value())
			{
				if (pairs[i].triggerActor == State::projectilePickupTriggerBody)
				{
					fmt::print("Player {} picked up projectile.\n", j);
					State::heldPowerUps[j] = PowerUpOptions::PROJECTILE;
					projectile_pickup.playSound();
				}

				else if (pairs[i].triggerActor == State::speedboostPickupTriggerBody)
				{
					fmt::print("Player {} picked up speed boost.\n", j);
					State::heldPowerUps[j] = PowerUpOptions::SPEED_BOOST;
					speed_boost_pickup.playSound();
				}

				else if (pairs[i].triggerActor == State::spikeTrapPickupTriggerBody)
				{
					fmt::print("Player {} picked up spike trap\n", i);
					State::heldPowerUps[i] = PowerUpOptions::SPIKE_TRAP;
					spike_trap_pickup.playSound();
				}
			}
		}

		//Button switch
		if (pairs[i].triggerActor == State::doorSwitchPickupTriggerBody && !State::arenaSwitch) {
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
			if ((pairHeader.actors[0] == State::vehicles[1]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[0]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[1]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[0]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[0]) {
					State::killCar0 = true;
					State::flagPickedUpBy[0] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[1]) {
					State::killCar1 = true;
					State::flagPickedUpBy[1] = false;
					State::flagPickedUp = false;
				}
				printf("Car 0 and Car 1 have hit\n");
			}
			if ((pairHeader.actors[0] == State::vehicles[2]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[0]->getRigidDynamicActor())|| (pairHeader.actors[1] == State::vehicles[2]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[0]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[0]) {
					State::killCar0 = true;
					State::flagPickedUpBy[0] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[2]) {
					State::killCar2 = true;
					State::flagPickedUpBy[2] = false;
					State::flagPickedUp = false;
				}
				printf("Car 0 and Car 2 have hit\n");
			}
			if ((pairHeader.actors[0] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[0]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[0]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[0]) {
					State::killCar0 = true;
					State::flagPickedUpBy[0] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[3]) {
					State::killCar3 = true;
					State::flagPickedUpBy[3] = false;
					State::flagPickedUp = false;
				}
				printf("Car 0 and Car 3 have hit\n");
			}
			if ((pairHeader.actors[0] == State::vehicles[2]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[1]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[2]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[1]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[1]) {
					State::killCar1 = true;
					State::flagPickedUpBy[1] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[2]) {
					State::killCar2 = true;
					State::flagPickedUpBy[2] = false;
					State::flagPickedUp = false;
				}
				printf("Car 1 and Car 2 have hit\n");
			}
			if ((pairHeader.actors[0] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[1]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[1]->getRigidDynamicActor())) {
				if (State::flagPickedUpBy[1]) {
					State::killCar1 = true;
					State::flagPickedUpBy[1] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[3]) {
					State::killCar3 = true;
					State::flagPickedUpBy[3] = false;
					State::flagPickedUp = false;
				}
				printf("Car 1 and Car 3 have hit\n");
			}
			if ((pairHeader.actors[0] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[1] == State::vehicles[2]->getRigidDynamicActor()) || (pairHeader.actors[1] == State::vehicles[3]->getRigidDynamicActor() && pairHeader.actors[0] == State::vehicles[2]->getRigidDynamicActor())){
				if (State::flagPickedUpBy[2]) {
					State::killCar2 = true;
					State::flagPickedUpBy[2] = false;
					State::flagPickedUp = false;
				}
				else if (State::flagPickedUpBy[3]) {
					State::killCar3 = true;
					State::flagPickedUpBy[3] = false;
					State::flagPickedUp = false;
				}
				printf("Car 2 and Car 3 have hit\n");
			}
			
		}
	}
}