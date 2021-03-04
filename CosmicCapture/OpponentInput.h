#pragma once
#include "Input.h"

class OpponentInput
{
public:
	OpponentInput(int playerNumber);
	std::map<MovementFlags, bool> getInput();
private:
	int playerNum;
	int counter;
};

