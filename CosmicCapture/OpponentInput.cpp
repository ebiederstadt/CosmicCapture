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
	if (counter % 60) {
		Pathfinding p;
		p.ehStarSearch(State::worldGrid, std::make_pair(0,0), std::make_pair(20, 23));
	}
	counter++;
	return inputMap;
}
