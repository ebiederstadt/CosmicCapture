#include "OpponentInput.h"

OpponentInput::OpponentInput(int playerNumber)
{
	playerNum = playerNumber;
	counter = 0;
}

std::map<MovementFlags, bool> OpponentInput::getInput(PxVec3 playerPos, PxVec3 playerDir) {
	std::pair<int, int> current = getGridCoordinates(playerPos.x, playerPos.z);

	if (current == target) {
		if (path.empty()) {
			path = pathfinder.ehStarSearch(State::worldGrid, current, std::make_pair<int,int>(10,10));
		}
		target = path.top();
		path.pop();
	}
	int targetDir = getTargetDirection(current, target);
	int playerOrientation = getOrientation(playerDir);
	return getCommand(targetDir, playerOrientation);
}

void OpponentInput::updatePath(PxVec3 playerPos, PxVec3 targetPos) {
	std::pair<int, int> p = getGridCoordinates(playerPos.x, playerPos.z);
	std::pair<int, int> t = getGridCoordinates(targetPos.x, targetPos.z);
	path = pathfinder.ehStarSearch(State::worldGrid, p, t);
	target = path.top();
	path.pop();
}
std::map<MovementFlags, bool> OpponentInput::followPath() {
	std::map<MovementFlags, bool> inputMap;
	return inputMap;
}

std::pair<int, int> OpponentInput::getGridCoordinates(float globalPosX, float globalPosZ) {
	int xIndex = std::min((int)((globalPosX + 100.f) / 10.f), 19);
	int zIndex = std::min((int)((globalPosZ + 100.f) / 10.f), 19);
	std::pair p(xIndex, zIndex);
	return p;
}


int OpponentInput::getTargetDirection(std::pair<int, int> playerCoords, std::pair<int, int> targetCoords) {
	int playerX = playerCoords.first;
	int playerY = playerCoords.second;
	int targetX = targetCoords.first;
	int targetY = targetCoords.second;
	int dir = 0;
	if (playerX == targetX && playerY > targetY) {
		dir = 7; //west
	}
	else if (playerX < targetX && playerY > targetY) {
		dir = 8; //northwest
	}
	else if (playerX < targetX && playerY == targetY) {
		dir = 1; //north
	}
	else if (playerX < targetX && playerY < targetY) {
		dir = 2; //northeast
	}
	else if (playerX == targetX && playerY < targetY) {
		dir = 3; //east
	}
	else if (playerX > targetX && playerY < targetY) {
		dir = 4;//southeast
	}
	else if (playerX > targetX && playerY == targetY) {
		dir = 5; //south
	}
	else if (playerX > targetX && playerY > targetY)  {
		dir = 6; //southwest
	}

	return dir;
}

int OpponentInput::getOrientation(PxVec3 dirVec) {
	int dir = 0;
	float x = dirVec.x;
	float z = dirVec.z;
	
	if (z < -0.90f)  {
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

std::map<MovementFlags, bool> OpponentInput::getCommand(int playerDir, int targetDir) {
	std::map<MovementFlags, bool> inputMap;
	if (playerDir == targetDir) {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		printf("STRAIGHT\n");
		return inputMap;
	}

	bool left = false;


	switch (playerDir) {
	case 1:
		switch (targetDir) {
		case 6:
			left = true;
			break;
		case 7:
			left = true;
			break;
		case 8:
			left = true;
			break;
		}
		break;
	case 2:
		switch (targetDir) {
		case 1:
			left = true;
			break;
		case 7:
			left = true;
			break;
		case 8:
			left = true;
			break;
		}
		break;
	case 3:
		switch (targetDir) {
		case 1:
			left = true;
			break;
		case 2:
			left = true;
			break;
		case 8:
			left = true;
			break;
		}
		break;
	case 4:
		switch (targetDir) {
		case 1:
			left = true;
			break;
		case 2:
			left = true;
			break;
		case 3:
			left = true;
			break;
		}
		break;
	case 5:
		switch (targetDir) {
		case 2:
			left = true;
			break;
		case 3:
			left = true;
			break;
		case 4:
			left = true;
			break;
		}
		break;
	case 6:
		switch (targetDir) {
		case 3:
			left = true;
			break;
		case 4:
			left = true;
			break;
		case 5:
			left = true;
			break;
		}
		break;
	case 7:
		switch (targetDir) {
		case 4:
			left = true;
			break;
		case 5:
			left = true;
			break;
		case 6:
			left = true;
			break;
		}
		break;
	case 8:
		switch (targetDir) {
		case 5:
			left = true;
			break;
		case 6:
			left = true;
			break;
		case 7:
			left = true;
			break;
		}
		break;
	}

	if (left) {
		inputMap[MovementFlags::LEFT] = false;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		printf("LEFT\n");
	}
	else {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = false;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
		printf("RIGHT\n");
	}
	return inputMap;
}