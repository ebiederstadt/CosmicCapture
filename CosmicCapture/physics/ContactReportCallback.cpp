#include "ContactReportCallback.h"

#include <fmt/format.h>
#include "../GlobalState.h"

void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].triggerActor == State::pickupBox && pairs[i].otherActor != State::flagBody && !State::flagPickedUp)
		{
			fmt::print("picked up flag\n");
			State::flagPickedUp = true;
		}
		if (pairs[i].triggerActor == State::dropoffBox && pairs[i].otherActor != State::flagBody && State::flagPickedUp)
		{
			fmt::print("dropped off flag\n");
			State::flagPickedUp = false;
		}
		if (pairs[i].triggerActor == State::projectilePickupTriggerBody && !State::projectilePickedUp) {
			fmt::print("projectile picked up\n");
			State::projectilePickedUp = true;
		}

	}
}
