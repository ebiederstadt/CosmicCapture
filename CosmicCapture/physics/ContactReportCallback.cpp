#include "ContactReportCallback.h"

#include <fmt/format.h>

void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].triggerActor == pickupBox && pairs[i].otherActor != flagBody && !flagPickedUp)
		{
			fmt::print("picked up flag\n");
			flagPickedUp = true;
		}
		if (pairs[i].triggerActor == dropoffBox && pairs[i].otherActor != flagBody && flagPickedUp)
		{
			fmt::print("dropped off flag\n");
			flagPickedUp = false;
		}
	}
}
