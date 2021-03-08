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
			if (pairs[i].otherActor == State::vehicleRDs[0] && !State::flagPickedUpBy[0] && !State::flagPickedUp) {
				State::flagPickedUpBy[0] = true;
				fmt::print("player 0 picked up flag\n");
			}
			else if (pairs[i].otherActor == State::vehicleRDs[1] && !State::flagPickedUpBy[1] && !State::flagPickedUp) {
				State::flagPickedUpBy[1] = true;
				fmt::print("player 1 picked up flag\n");
			}
			else if (pairs[i].otherActor == State::vehicleRDs[2] && !State::flagPickedUpBy[2] && !State::flagPickedUp) {
				State::flagPickedUpBy[2] = true;
				fmt::print("player 2 picked up flag\n");
			}
			else if (pairs[i].otherActor == State::vehicleRDs[3] && !State::flagPickedUpBy[3] && !State::flagPickedUp) {
				State::flagPickedUpBy[3] = true;
				fmt::print("player 3 picked up flag\n");
			}
			State::flagPickedUp = true;
		}

		// Flag dropoffs
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[0] && pairs[i].otherActor == State::vehicleRDs[0] && State::flagPickedUpBy[0])
		{
			fmt::print("player 0 dropped off flag\n");
			State::flagPickedUpBy[0] = false;
			State::flagPickedUp = false;
			State::scores[0]++;
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[1] && pairs[i].otherActor == State::vehicleRDs[1] && State::flagPickedUpBy[1])
		{
			fmt::print("player 1 dropped off flag\n");
			State::flagPickedUpBy[1] = false;
			State::flagPickedUp = false;
			State::scores[1]++;
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[2] && pairs[i].otherActor == State::vehicleRDs[2] && State::flagPickedUpBy[2])
		{
			fmt::print("player 2 dropped off flag\n");
			State::flagPickedUpBy[2] = false;
			State::flagPickedUp = false;
			State::scores[2]++;
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[3] && pairs[i].otherActor == State::vehicleRDs[3] && State::flagPickedUpBy[3])
		{
			fmt::print("player 3 dropped off flag\n");
			State::flagPickedUpBy[3] = false;
			State::flagPickedUp = false;
			State::scores[3]++;
		}

		// Powerups
		else if (pairs[i].triggerActor == State::projectilePickupTriggerBody && !State::projectilePickedUp) {
			fmt::print("projectile picked up\n");
			State::projectilePickedUp = true;
		}
		else if (pairs[i].triggerActor == State::speedboostPickupTriggerBody && !State::speedboostPickedUp) {
			fmt::print("speed boost picked up\n");
			State::speedboostPickedUp = true;
		}
		else if (pairs[i].triggerActor == State::spikeTrapPickupTriggerBody && !State::spikeTrapPickedUp) {
			fmt::print("Spike trap picked up\n");
			State::spikeTrapPickedUp = true;
		}
		else if (pairs[i].triggerActor == State::spikeTrapTriggerBody && State::spikeTrapActive) {
			fmt::print("Ran into spike trap!\n");
			State::spikeTrapActive = false;
			State::spikeTrapInUse = true;

			for (int j = 0; j < 4; ++j)
			{
				if (pairs[i].otherActor == State::vehicleRDs[j])
				{
					State::spikeTrapActingUpon = j;
					break;
				}
					
			}
		}

	}
}
