#include "ContactReportCallback.h"

#include <fmt/format.h>
#include "../GlobalState.h"

void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		// Flag pickups
		if (pairs[i].triggerActor == State::pickupBox && pairs[i].otherActor != State::flagBody)
		{		
			if (pairs[i].otherActor == State::vehicles[0]->getRigidDynamicActor() && !State::flagPickedUpBy[0] && !State::flagPickedUp) {
				State::flagPickedUpBy[0] = true;
				fmt::print("player 0 picked up flag\n");
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
		else if (pairs[i].triggerActor == State::projectilePickupTriggerBody && !State::heldPowerUps[i].has_value()) {
			fmt::print("Player {} picked up projectile.\n", i);
			State::heldPowerUps[i] = PowerUpOptions::PROJECTILE;
		}
		else if (pairs[i].triggerActor == State::speedboostPickupTriggerBody && !State::heldPowerUps[i].has_value()) {
			fmt::print("Player {} picked up speed boost.\n", i);
			State::heldPowerUps[i] = PowerUpOptions::SPEED_BOOST;
		}

		if (pairs[i].triggerActor == State::spikeTrapPickupTriggerBody && !State::heldPowerUps[i].has_value()) {
			fmt::print("Player {} picked up spike trap\n", i);
			State::heldPowerUps[i] = PowerUpOptions::SPIKE_TRAP;
		}

		for (auto& spikeTrapState : State::spike_trap_states)
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
