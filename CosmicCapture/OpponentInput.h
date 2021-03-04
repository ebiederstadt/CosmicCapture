#pragma once
#include "Input.h"
#include "Pathfinding.h"
#include "GlobalState.h"

class OpponentInput
{
public:
	OpponentInput(int playerNumber);
	std::map<MovementFlags, bool> getInput();
private:
	int playerNum;
	int counter;
};

