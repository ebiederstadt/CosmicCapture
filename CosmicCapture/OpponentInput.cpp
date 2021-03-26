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



std::map<MovementFlags, bool> OpponentInput::getInput(PxVec3 playerPos, PxVec3 playerDir) {
	float vehicleSpeed = mVehicles[0]->computeForwardSpeed();
	std::pair<int, int> current = getGridCoordinates(playerPos.x, playerPos.z);
	if (current == target) {
		if (path.empty()) {
			path = pathfinder.ehStarSearch(State::worldGrid, current, std::make_pair<int,int>(10,10)); //FIX THIS PROBLEM
		}
		target = path.top();
		path.pop();
	}

	if (current == lastPosition && !reversing) {
		if (stuckCounter >= stuckThreshold) {
			reversing = true;
		}
		else {
			stuckCounter++;
		}
	}
	else {
		stuckCounter = 0;
		lastPosition = current;
	}

	std::map<MovementFlags, bool> command;
	if (reversing) {
		command[MovementFlags::LEFT] = true;
		command[MovementFlags::RIGHT] = true;
		command[MovementFlags::DOWN] = false;
		command[MovementFlags::UP] = true;
		if (vehicleSpeed < reverseThresholdSpeed) {
			reversing = false;
		}
	}
	else {
		if (target.first == -1 && target.second == -1) {
			command[MovementFlags::LEFT] = true;
			command[MovementFlags::RIGHT] = true;
			command[MovementFlags::DOWN] = true;
			command[MovementFlags::UP] = false;
		}
		else {
			PxVec3 targetDir = getPlayerToTargetDir(playerDir, playerNum);
			command = getCommand(dirsToCommand(playerDir, targetDir));
		}	
	}
	
	//printf("(%d, %d), (%d, %d) - %d, %d\n", current.first, current.second, target.first, target.second, stuckCounter, reverseCounter);
	counter++;
	return command;
}

void OpponentInput::updatePath(PxVec3 playerPos, PxVec3 targetPos) {
	std::pair<int, int> p = getGridCoordinates(playerPos.x, playerPos.z);
	std::pair<int, int> t = getGridCoordinates(targetPos.x, targetPos.z);
	if (p == t) {

		
	}
	else {
		path = pathfinder.ehStarSearch(State::worldGrid, p, t);
		target = path.top();
		path.pop();
	}
}
std::map<MovementFlags, bool> OpponentInput::followPath() {
	std::map<MovementFlags, bool> inputMap;
	return inputMap;
}

std::pair<int, int> OpponentInput::getGridCoordinates(float globalPosX, float globalPosZ) {
	int xIndex = std::min((int)((globalPosX + 180.f) / 10.f), 35);
	int zIndex = std::min((int)((globalPosZ + 180.f) / 10.f), 35);
	std::pair p(xIndex, zIndex); 
	return p;
}






PxVec3 OpponentInput::getPlayerToTargetDir(PxVec3 playerDirVec, int playerVehicleRDIndex, PxVec3 targetGlobalPos) {
	std::pair<int, int> targetGridPos = getGridCoordinates(targetGlobalPos.x, targetGlobalPos.y);

	float targetX = State::worldGridCenterCoords[targetGridPos.first][targetGridPos.second].first;
	float targetZ = State::worldGridCenterCoords[targetGridPos.first][targetGridPos.second].second;
	PxVec3 playerPos = State::vehicleRDs[playerVehicleRDIndex]->getGlobalPose().p;
	float playerX = playerPos.x;
	float playerZ = playerPos.z;

	float shiftedTargetX = targetX - playerX;
	float shiftedTargetZ = targetZ - playerZ;

	return PxVec3(shiftedTargetX, 0.f, shiftedTargetZ).getNormalized();

}
PxVec3 OpponentInput::getPlayerToTargetDir(PxVec3 playerDirVec, int playerVehicleRDIndex) {
	float targetX = State::worldGridCenterCoords[target.first][target.second].first;
	float targetZ = State::worldGridCenterCoords[target.first][target.second].second;
	PxVec3 playerPos = State::vehicleRDs[playerVehicleRDIndex]->getGlobalPose().p;
	float playerX = playerPos.x;
	float playerZ = playerPos.z;

	float shiftedTargetX = targetX - playerX;
	float shiftedTargetZ = targetZ - playerZ;

	return PxVec3(shiftedTargetX, 0.f, shiftedTargetZ).getNormalized();

}

int OpponentInput::dirsToCommand(PxVec3 playerDirVec, PxVec3 targetDirVec) {
	float playerX = playerDirVec.x;
	float playerZ = playerDirVec.z;
	float targetX = targetDirVec.x;
	float targetZ = targetDirVec.z;
	float playerAngleRads = atan2(playerZ, playerX);
	float targetAngleRads = atan2(targetZ, targetX);
	int commandNum = 1;

	if (abs(playerAngleRads - targetAngleRads) < 0.175f) { //TUNING POINT -- magic number = how close you have to be pointing to your target to go straight
		return commandNum;
	}
	if (playerAngleRads > targetAngleRads) {
		if ((double)playerAngleRads - targetAngleRads > M_PI) {
			commandNum = 2;
		}
		else {
			commandNum = 3;
		}
	}
	else {
		if ((double)targetAngleRads - playerAngleRads > M_PI) {
			commandNum = 3;
		}
		else {
			commandNum = 2;
		}
	}
	return commandNum;
}

std::map<MovementFlags, bool> OpponentInput::getCommand(int commandNum) {
	std::map<MovementFlags, bool> inputMap;
	if (commandNum == 1) {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		//printf("STRAIGHT\n");
	}
	else if (commandNum == 3) {
		inputMap[MovementFlags::LEFT] = false;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		//printf("LEFT\n");
	}
	else if (commandNum == 2) {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = false;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		//printf("RIGHT\n");
	}
	return inputMap;
}

void OpponentInput::setPlayerNum(int num) {
	playerNum = num;
}
/*
std::map<MovementFlags, bool> OpponentInput::getCommand(int targetDir, int playerDir) {
	std::map<MovementFlags, bool> inputMap;
	int command = actionArray[targetDir - 1][playerDir - 1];
	//printf("%d, %d, %d ", playerDir, targetDir, command);
	if (command == 1) {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		//printf("STRAIGHT\n");
	}
	else if (command == 3) {
		inputMap[MovementFlags::LEFT] = false;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		//printf("LEFT\n");
	}
	else if (command == 2) {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = false;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		//printf("RIGHT\n");
	}
	return inputMap;
}
*/



/*
int OpponentInput::getTargetDirection(std::pair<int, int> playerCoords, std::pair<int, int> targetCoords) {
	int playerX = playerCoords.first;
	int playerY = playerCoords.second;
	int targetX = targetCoords.first;
	int targetY = targetCoords.second;
	int dir = 0;
	if (playerX == targetX && playerY > targetY) {
		dir = 3; //east
	}
	else if (playerX < targetX && playerY > targetY) {
		dir = 4;//southeast
	}
	else if (playerX < targetX && playerY == targetY) {
		dir = 5; //south
	}
	else if (playerX < targetX && playerY < targetY) {
		dir = 6; //southwest
	}
	else if (playerX == targetX && playerY < targetY) {
		dir = 7; //west
	}
	else if (playerX > targetX && playerY < targetY) {
		dir = 8; //northwest
	}
	else if (playerX > targetX && playerY == targetY) {
		dir = 1; //north
	}
	else if (playerX > targetX && playerY > targetY) {
		dir = 2; //northeast
	}

	return dir;
}

int OpponentInput::checkDiagonals(std::pair<int, int> currentPos, int targetDir) {
	int dir = targetDir;
	int right = -1;
	int left = -1;

	switch (targetDir) {
	case 2:
		right = State::worldGrid[currentPos.first][currentPos.second - 1];
		left = State::worldGrid[currentPos.first - 1][currentPos.second];
		if (right == 0 && left == 1) {
			dir = 1;
		}
		else if (right == 1 && left == 0) {
			dir = 3;
		}
		break;
	case 4:
		right = State::worldGrid[currentPos.first + 1][currentPos.second];
		left = State::worldGrid[currentPos.first][currentPos.second - 1];
		if (right == 0 && left == 1) {
			dir = 3;
		}
		else if (right == 1 && left == 0) {
			dir = 5;
		}
		break;
	case 6:
		right = State::worldGrid[currentPos.first][currentPos.second + 1];
		left = State::worldGrid[currentPos.first + 1][currentPos.second];
		if (right == 0 && left == 1) {
			dir = 5;
		}
		else if (right == 1 && left == 0) {
			dir = 7;
		}
		break;
	case 8:
		right = State::worldGrid[currentPos.first - 1][currentPos.second];
		left = State::worldGrid[currentPos.first][currentPos.second + 1];
		if (right == 0 && left == 1) {
			dir = 7;
		}
		else if (right == 1 && left == 0) {
			dir = 1;
		}
		break;
	}

	return dir;
}


int OpponentInput::getOrientation(PxVec3 dirVec) {
	int dir = 0;
	float x = dirVec.x;
	float z = dirVec.z;

	if (z < -0.90f) {
		dir = 3; //EAST 
	}
	else if (z < 0.f && x >= 0.f && x < 0.90f) {
		dir = 4; //SOUTHEAST
	}
	else if (x > 0.90f) {
		dir = 5; //SOUTH
	}
	else if (z > 0.f && z < 0.90f && x >= 0.f && x < 0.90f) {
		dir = 6; //SOUTHWEST
	}
	else if (z > 0.90f) {
		dir = 7; //WEST
	}
	else if (z > 0.f && x < 0.f && x > -0.90f) {
		dir = 8; //NORTHWEST
	}
	else if (x <= -0.90f) {
		dir = 1; //NORTH
	}
	else if (z < 0.f && x < 0.f && x > -0.90f) {
		dir = 2; //NORTHEAST
	}

	return dir;
}
*/