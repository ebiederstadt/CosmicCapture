#pragma once
#include "physx/PxContactModifyCallback.h"

class ContactModification : public physx::PxContactModifyCallback
{
	void onContactModify(physx::PxContactModifyPair* const pairs, physx::PxU32 count);
};


