#include "ContactReportCallback.h"

#include <fmt/format.h>
#include "../GlobalState.h"

void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].triggerActor == State::pickupBox && pairs[i].otherActor != State::flagBody)
		{		
			if (pairs[i].otherActor == State::vehicleRDs[0] && !State::flagPickedUp[0]) {
				State::flagPickedUp[0] = true;
				fmt::print("player 0 picked up flag\n");
			}
			else if (pairs[i].otherActor == State::vehicleRDs[1] && !State::flagPickedUp[1]) {
				State::flagPickedUp[1] = true;
				fmt::print("player 1 picked up flag\n");
			}
			else if (pairs[i].otherActor == State::vehicleRDs[2] && !State::flagPickedUp[2]) {
				State::flagPickedUp[2] = true;
				fmt::print("player 2 picked up flag\n");
			}
			else if (pairs[i].otherActor == State::vehicleRDs[3] && !State::flagPickedUp[3]) {
				State::flagPickedUp[3] = true;
				fmt::print("player 3 picked up flag\n");
			}

		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[0] && pairs[i].otherActor == State::vehicleRDs[0] && State::flagPickedUp[0])
		{
			fmt::print("player 0 dropped off flag\n");
			State::flagPickedUp[0] = false;
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[1] && pairs[i].otherActor == State::vehicleRDs[1] && State::flagPickedUp[1])
		{
			fmt::print("player 1 dropped off flag\n");
			State::flagPickedUp[1] = false;
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[2] && pairs[i].otherActor == State::vehicleRDs[2] && State::flagPickedUp[2])
		{
			fmt::print("player 2 dropped off flag\n");
			State::flagPickedUp[2] = false;
		}
		else if (pairs[i].triggerActor == State::flagDropoffBoxes[3] && pairs[i].otherActor == State::vehicleRDs[3] && State::flagPickedUp[3])
		{
			fmt::print("player 3 dropped off flag\n");
			State::flagPickedUp[3] = false;
		}
		else if (pairs[i].triggerActor == State::projectilePickupTriggerBody && !State::projectilePickedUp) {
			fmt::print("projectile picked up\n");
			State::projectilePickedUp = true;
		}

	}
}
