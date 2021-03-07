#include "OpponentInput.h"

OpponentInput::OpponentInput(int playerNumber)
{
	playerNum = playerNumber;
	counter = 0;
}

std::map<MovementFlags, bool> OpponentInput::getInput() {
	std::map<MovementFlags, bool> inputMap;
	inputMap[MovementFlags::LEFT] = true;
	inputMap[MovementFlags::RIGHT] = true;
	inputMap[MovementFlags::DOWN] = true;
	inputMap[MovementFlags::UP] = true;

	return inputMap;
}

std::map<MovementFlags, bool> OpponentInput::followPath() {
	std::map<MovementFlags, bool> inputMap;
	return inputMap;
}

std::pair<int, int> OpponentInput::getGridCoordinates(float globalPosX, float globalPosZ) {
	int xIndex = (int)((globalPosX + 100.f) / 10.f);
	int zIndex = (int)((globalPosZ + 100.f) / 10.f);
	std::pair p(xIndex, zIndex);
	return p;
}


int OpponentInput::getOrientation(PxVec3 dirVec) {
	int dir = 0;
	float x = dirVec.x;
	float z = dirVec.z;
	
	if (z < -0.90f)  {
		dir = 1; //NORTH
	}
	else if (z < 0.f && x >= 0.f && x < 0.90f) {
		dir = 2; //NORTHEAST
	}
	else if (x > 0.90f) {
		dir = 3; //EAST
	}
	else if (z > 0.f && z < 0.90f && x >= 0.f && x < 0.90f) {
		dir = 4; //SOUTHEAST
	}
	else if (z > 0.90f) {
		dir = 5; //SOUTH
	}
	else if (z > 0.f && x < 0.f && x > -0.90f) {
		dir = 6; //SOUTHWEST
	}
	else if (x <= -0.90f) {
		dir = 7; //WEST
	}
	else if (z < 0.f && x < 0.f && x > -0.90f) {
		dir = 8; //NORTHWEST
	}
	
	return dir;
}

std::map<MovementFlags, bool> OpponentInput::getDirection(int playerDir, int targetDir) {
	std::map<MovementFlags, bool> inputMap;
	if (playerDir == targetDir) {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
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
	}
	else {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = false;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	return inputMap;
}