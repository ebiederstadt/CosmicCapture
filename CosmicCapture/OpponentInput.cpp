#include "OpponentInput.h"

OpponentInput::OpponentInput()
{
	playerNum = 0;
	counter = 0;
	stuckCounter = 0;
}

OpponentInput::OpponentInput(int playerNumber)
{
	playerNum = playerNumber;
	counter = 0;
	stuckCounter = 0;
}


std::map<MovementFlags, bool> OpponentInput::getInput(PxVec3 playerPos, PxVec3 playerDir)
{
	bool sharpTurn = false;
	float vehicleSpeed = mVehicles[0]->computeForwardSpeed();
	std::pair<int, int> current = getGridCoordinates(playerPos.x, playerPos.z);
	std::pair<float, float> centre = worldGridCenterCoords[target.first][target.second];
	double dist = std::sqrt(std::pow(playerPos.x - centre.first, 2) + std::pow(playerPos.z - centre.second, 2));
	double distToGoal = std::sqrt(std::pow(playerPos.x - goalPos.x, 2) + std::pow(playerPos.z - goalPos.z, 2));
	if (distToGoal < 25.f)
	{
		subTargetting = true;
	}
	else
	{
		subTargetting = false;
	}
	if (subTargetting)
	{
		if (distToGoal < 1.f)
		{
			subTargetting = false;
			if (path.empty())
			{
				path = pathfinder.aStarSearch(worldGrid, current, getGridCoordinates(goalPos.x, goalPos.z));
			}
			target = path.top();
			path.pop();
		}
	}
	else
	{
		if (dist < 10.f)
		{
			if (path.empty())
			{
				path = pathfinder.aStarSearch(worldGrid, current, getGridCoordinates(goalPos.x, goalPos.z));
			}
			target = path.top();
			path.pop();
		}
	}


	if (vehicleSpeed < 0.5f && !reversing)
	{
		if (stuckCounter >= stuckThreshold)
		{
			reversing = true;
		}
		else
		{
			stuckCounter++;
		}
	}
	else
	{
		stuckCounter = 0;
		lastPosition = current;
	}

	std::map<MovementFlags, bool> command;
	if (reversing)
	{
		command[MovementFlags::LEFT] = true;
		command[MovementFlags::RIGHT] = true;
		command[MovementFlags::DOWN] = false;
		command[MovementFlags::UP] = true;
		framesReversing++;
		if (vehicleSpeed < reverseThresholdSpeed || framesReversing > reversingMax)
		{
			reversing = false;
			framesReversing = 0;
		}
	}
	else
	{
		if (target.first == -1 && target.second == -1)
		{
			command[MovementFlags::LEFT] = true;
			command[MovementFlags::RIGHT] = true;
			command[MovementFlags::DOWN] = true;
			command[MovementFlags::UP] = false;
		}
		else if (target.first == -2 && target.second == -2)
		{
			command[MovementFlags::LEFT] = true;
			command[MovementFlags::RIGHT] = true;
			command[MovementFlags::DOWN] = true;
			command[MovementFlags::UP] = true;
		}
		else
		{
			PxVec3 targetDir;
			if (subTargetting)
			{
				targetDir = getPlayerToTargetDir(playerNum, goalPos);
			}
			else
			{
				targetDir = getPlayerToTargetDir(playerNum);
			}
			command = getCommand(dirsToCommand(playerDir, targetDir, &sharpTurn));
		}
	}

	command[MovementFlags::ACTION] = true;
	if (!recentlyUsedAction)
	{
		if (State::heldPowerUps[playerNum] == PowerUpOptions::SPIKE_TRAP)
		{
			command[MovementFlags::ACTION] = false;
			recentlyUsedAction = true;
		}
		else if (State::heldPowerUps[playerNum] == PowerUpOptions::SPEED_BOOST)
		{
			command[MovementFlags::ACTION] = false;
			recentlyUsedAction = true;
		}
		else if (State::heldPowerUps[playerNum] == PowerUpOptions::PROJECTILE)
		{
			if (State::flagPickedUp)
			{
				if (State::flagPickedUpBy[playerNum])
				{
				}
				else
				{
					if (subTargetting && pointingAtGoal(playerDir, getPlayerToTargetDir(playerNum, goalPos)))
					{
						command[MovementFlags::ACTION] = false;
						recentlyUsedAction = true;
					}
				}
			}
		}
	}
	else
	{
		actionCounter++;
		if (actionCounter > actionDelay)
		{
			actionCounter = 0;
			recentlyUsedAction = false;
		}
	}
	return command;
}


void OpponentInput::updatePath(PxVec3 playerPos, PxVec3 targetPos)
{
	std::pair<int, int> p = getGridCoordinates(playerPos.x, playerPos.z);
	std::pair<int, int> t = getGridCoordinates(targetPos.x, targetPos.z);
	goalPos = targetPos;
	if (p == t)
	{
	}
	else if (worldGrid[p.first][p.second] == 0 || worldGrid[t.first][t.second] == 0)
	{
	}
	else
	{
		path = pathfinder.aStarSearch(worldGrid, p, t);
		if (path.size() > 1)
		{
			path.pop();
		}
		target = path.top();
		path.pop();
	}
}

std::map<MovementFlags, bool> OpponentInput::followPath()
{
	std::map<MovementFlags, bool> inputMap;
	return inputMap;
}

std::pair<int, int> OpponentInput::getGridCoordinates(float globalPosX, float globalPosZ) const
{
	int xIndex = std::min(static_cast<int>((globalPosX + 325.f) / 25.f), 25);
	int zIndex = std::min(static_cast<int>((globalPosZ + 325.f) / 25.f), 25);
	std::pair p(xIndex, zIndex);
	return p;
}


PxVec3 OpponentInput::getPlayerToTargetDir(int playerVehicleRDIndex, PxVec3 targetGlobalPos)
{
	float targetX = targetGlobalPos.x;
	float targetZ = targetGlobalPos.z;
	PxVec3 playerPos = State::vehicles[playerVehicleRDIndex]->getRigidDynamicActor()->getGlobalPose().p;
	float playerX = playerPos.x;
	float playerZ = playerPos.z;

	float shiftedTargetX = targetX - playerX;
	float shiftedTargetZ = targetZ - playerZ;

	return PxVec3(shiftedTargetX, 0.f, shiftedTargetZ).getNormalized();
}

PxVec3 OpponentInput::getPlayerToTargetDir(int playerVehicleRDIndex) const
{
	float targetX = worldGridCenterCoords[target.first][target.second].first;
	float targetZ = worldGridCenterCoords[target.first][target.second].second;
	PxVec3 playerPos = State::vehicles[playerVehicleRDIndex]->getRigidDynamicActor()->getGlobalPose().p;
	float playerX = playerPos.x;
	float playerZ = playerPos.z;

	float shiftedTargetX = targetX - playerX;
	float shiftedTargetZ = targetZ - playerZ;

	return PxVec3(shiftedTargetX, 0.f, shiftedTargetZ).getNormalized();
}

bool OpponentInput::pointingAtGoal(PxVec3 playerDirVec, PxVec3 targetDirVec) const
{
	bool pointingAtGoal = false;
	float playerX = playerDirVec.x;
	float playerZ = playerDirVec.z;
	float targetX = targetDirVec.x;
	float targetZ = targetDirVec.z;
	float playerAngleRads = atan2(playerZ, playerX);
	float targetAngleRads = atan2(targetZ, targetX);
	float diff = abs(playerAngleRads - targetAngleRads);
	if (diff < 0.1f)
	{
		pointingAtGoal = true;
	}
	return pointingAtGoal;
}

void initializeGridCenterCoords()
{
	float flatOffset = 10.f; //TUNING POINT
	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			worldGridCenterCoords[i][j].first = i * 25.f - 325.f + 12.5f;
			worldGridCenterCoords[i][j].second = j * 25.f - 325.f + 12.5f;
			if (i == 1 || j == 1 || j == 24 || i == 24) continue;
			if ((i + 1 < 26) && (i - 1 >= 0) && (j + 1 < 26) && (j - 1 >= 0))
			{
				if (worldGrid[i + 1][j] == 0)
					worldGridCenterCoords[i][j].first -= flatOffset;
				if (worldGrid[i - 1][j] == 0)
					worldGridCenterCoords[i][j].first += flatOffset;
				if (worldGrid[i][j + 1] == 0)
					worldGridCenterCoords[i][j].second -= flatOffset;
				if (worldGrid[i][j - 1] == 0)
					worldGridCenterCoords[i][j].second += flatOffset;
			}
		}
	}
}

void updateWorldGridArenaToRedArena()
{
	worldGrid[6][1] = 1;
	worldGrid[6][2] = 1;
	worldGrid[6][23] = 1;
	worldGrid[6][24] = 1;
	worldGrid[7][8] = 1;
	worldGrid[7][9] = 1;
	worldGrid[7][10] = 1;
	worldGrid[7][15] = 1;
	worldGrid[7][16] = 1;
	worldGrid[7][17] = 1;
	worldGrid[19][1] = 1;
	worldGrid[19][2] = 1;
	worldGrid[19][23] = 1;
	worldGrid[19][24] = 1;
	worldGrid[18][8] = 1;
	worldGrid[18][9] = 1;
	worldGrid[18][10] = 1;
	worldGrid[18][15] = 1;
	worldGrid[18][16] = 1;
	worldGrid[18][17] = 1;
	worldGrid[10][12] = 1;
	worldGrid[10][13] = 1;
	worldGrid[15][12] = 1;
	worldGrid[15][13] = 1;

	worldGrid[1][6] = 0;
	worldGrid[2][6] = 0;
	worldGrid[23][6] = 0;
	worldGrid[24][6] = 0;
	worldGrid[8][7] = 0;
	worldGrid[9][7] = 0;
	worldGrid[10][7] = 0;
	worldGrid[15][7] = 0;
	worldGrid[16][7] = 0;
	worldGrid[17][7] = 0;
	worldGrid[1][19] = 0;
	worldGrid[2][19] = 0;
	worldGrid[23][19] = 0;
	worldGrid[24][19] = 0;
	worldGrid[8][18] = 0;
	worldGrid[9][18] = 0;
	worldGrid[10][18] = 0;
	worldGrid[15][18] = 0;
	worldGrid[16][18] = 0;
	worldGrid[17][18] = 0;
	worldGrid[12][10] = 0;
	worldGrid[13][10] = 0;
	worldGrid[12][15] = 0;
	worldGrid[13][15] = 0;
}

void updateWorldGridArenaToBlueArena()
{
	worldGrid[6][1] = 0;
	worldGrid[6][2] = 0;
	worldGrid[6][23] = 0;
	worldGrid[6][24] = 0;
	worldGrid[7][8] = 0;
	worldGrid[7][9] = 0;
	worldGrid[7][10] = 0;
	worldGrid[7][15] = 0;
	worldGrid[7][16] = 0;
	worldGrid[7][17] = 0;
	worldGrid[19][1] = 0;
	worldGrid[19][2] = 0;
	worldGrid[19][23] = 0;
	worldGrid[19][24] = 0;
	worldGrid[18][8] = 0;
	worldGrid[18][9] = 0;
	worldGrid[18][10] = 0;
	worldGrid[18][15] = 0;
	worldGrid[18][16] = 0;
	worldGrid[18][17] = 0;
	worldGrid[10][12] = 0;
	worldGrid[10][13] = 0;
	worldGrid[15][12] = 0;
	worldGrid[15][13] = 0;

	worldGrid[1][6] = 1;
	worldGrid[2][6] = 1;
	worldGrid[23][6] = 1;
	worldGrid[24][6] = 1;
	worldGrid[8][7] = 1;
	worldGrid[9][7] = 1;
	worldGrid[10][7] = 1;
	worldGrid[15][7] = 1;
	worldGrid[16][7] = 1;
	worldGrid[17][7] = 1;
	worldGrid[1][19] = 1;
	worldGrid[2][19] = 1;
	worldGrid[23][19] = 1;
	worldGrid[24][19] = 1;
	worldGrid[8][18] = 1;
	worldGrid[9][18] = 1;
	worldGrid[10][18] = 1;
	worldGrid[15][18] = 1;
	worldGrid[16][18] = 1;
	worldGrid[17][18] = 1;
	worldGrid[12][10] = 1;
	worldGrid[13][10] = 1;
	worldGrid[12][15] = 1;
	worldGrid[13][15] = 1;
}

int OpponentInput::dirsToCommand(PxVec3 playerDirVec, PxVec3 targetDirVec, bool* sharpTurnFlag) const
{
	float playerX = playerDirVec.x;
	float playerZ = playerDirVec.z;
	float targetX = targetDirVec.x;
	float targetZ = targetDirVec.z;
	float playerAngleRads = atan2(playerZ, playerX);
	float targetAngleRads = atan2(targetZ, targetX);
	int commandNum = 1;
	float diff = abs(playerAngleRads - targetAngleRads);
	if (diff < 0.175f)
	{
		//TUNING POINT -- magic number = how close you have to be pointing to your target to go straight
		return commandNum;
	}
	if (diff > 1.f)
	{
		*sharpTurnFlag = true;
	}

	if (playerAngleRads > targetAngleRads)
	{
		if (static_cast<double>(playerAngleRads) - targetAngleRads > M_PI)
			commandNum = 2;
		else
			commandNum = 3;
	}
	else
	{
		if (static_cast<double>(targetAngleRads) - playerAngleRads > M_PI)
			commandNum = 3;
		else
			commandNum = 2;
	}
	return commandNum;
}

std::map<MovementFlags, bool> OpponentInput::getCommand(int commandNum)
{
	std::map<MovementFlags, bool> inputMap;
	if (commandNum == 1)
	{
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	else if (commandNum == 3)
	{
		inputMap[MovementFlags::LEFT] = false;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	else if (commandNum == 2)
	{
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = false;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	return inputMap;
}

void OpponentInput::setPlayerNum(int num)
{
	playerNum = num;
}

