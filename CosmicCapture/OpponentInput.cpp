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
	counter++;

	return inputMap;
}
