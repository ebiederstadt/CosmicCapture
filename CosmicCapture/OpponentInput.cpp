#include "OpponentInput.h"

OpponentInput::OpponentInput(int playerNumber)
{
	playerNum = playerNumber;
	counter = 0;
}

std::map<MovementFlags, bool> OpponentInput::getInput() {
	std::map<MovementFlags, bool> inputMap;
	if (counter % 3 == 0) {
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	else {
		inputMap[MovementFlags::LEFT] = false;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	counter++;
	if (counter > 100000)
		counter = 0;

	return inputMap;
}
