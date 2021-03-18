#pragma once
#include "Input.h"
#include "Pathfinding.h"
#include "GlobalState.h"
#include <algorithm>

class OpponentInput
{
public:
	OpponentInput();
	OpponentInput(int playerNumber);
	std::map<MovementFlags, bool> getInput(PxVec3 playerPos, PxVec3 playerDir);
	int getOrientation(PxVec3 dirVec);
	void updatePath(PxVec3 playerPos, PxVec3 targetPos);
private:
	
	std::map<MovementFlags, bool> followPath();
	std::pair<int, int> getGridCoordinates(float globalPosX, float globalPosZ);
	std::map<MovementFlags, bool> getCommand(int playerDir, int targetDir);
	int getTargetDirection(std::pair<int, int> playerCoords, std::pair<int, int> targetCoords);
	

	int playerNum;
	int counter;
	std::stack<std::pair<int, int>> path;
	std::pair<int, int> target;
	Pathfinding pathfinder;

	bool reversing = false;
	int reverseCounter;
	int maxReverseCount = 300;
	int stuckCounter;
	int stuckThreshold = 500;
	std::pair<int, int> lastPosition;

};

