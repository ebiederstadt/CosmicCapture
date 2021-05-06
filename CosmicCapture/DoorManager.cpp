#include "DoorManager.h"

#include "GlobalState.h"

void DoorManager::simulate()
{
	float timeDiff = arenaTimer - (AFFECTION_TIME - SWITCH_INTERVAL);
	gateDownOffset = -timeDiff * ABS_FRAME_GATE_OFFSET;
	gateUpOffset = -(SWITCH_INTERVAL - timeDiff) * ABS_FRAME_GATE_OFFSET;

	if (timeDiff == -25.f) Audio::gate_switch.playSound();

	arenaTimer += 1;
	if (arenaTimer >= AFFECTION_TIME)
	{
		arenaSwitch = true;
	}
}

