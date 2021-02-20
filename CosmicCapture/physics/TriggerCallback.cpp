#include "TriggerCallback.h"
#include <iostream>

void TriggerCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
    for (physx::PxU32 i = 0; i < count; i++)
    {
        printf("asdasdasdads \n");
    }
};