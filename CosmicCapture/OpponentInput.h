#pragma once
#include "Input.h"
#include "Pathfinding.h"
#include "GlobalState.h"

class OpponentInput
{
public:
	OpponentInput(int playerNumber);
	std::map<MovementFlags, bool> getInput();
	int getOrientation(PxVec3 dirVec);
private:
	
	std::map<MovementFlags, bool> followPath();
	std::pair<int, int> getGridCoordinates(float globalPosX, float globalPosZ);
	std::map<MovementFlags, bool> getDirection(int playerDir, int targetDir);

	int playerNum;
	int counter;
	std::stack<std::pair<int, int>> path;
	std::pair<int, int> current;
};

