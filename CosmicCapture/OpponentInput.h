#pragma once
#include "Input.h"
#include "PathFinding.h"
#include "GlobalState.h"
#include "PowerUpManager.h"

class OpponentInput
{
public:
	OpponentInput();
	OpponentInput(int);
	std::map<MovementFlags, bool> getInput(PxVec3, PxVec3);
	void updatePath(PxVec3, PxVec3);
	static PxVec3 getPlayerToTargetDir(int, PxVec3);
	PxVec3 getPlayerToTargetDir(int) const;
	int dirsToCommand(PxVec3, PxVec3, bool*) const;
	void setPlayerNum(int);
	void attachVehicle(PxVehicleDrive4W* vehicle) { mVehicles.push_back(vehicle); }

private:
	std::vector<PxVehicleDrive4W*> mVehicles;
	static std::map<MovementFlags, bool> followPath();
	std::pair<int, int> getGridCoordinates(float, float) const;
	std::map<MovementFlags, bool> getCommand(int);
	bool pointingAtGoal(PxVec3, PxVec3) const;

	int playerNum;
	int counter;
	std::stack<std::pair<int, int>> path;
	std::pair<int, int> target;
	PathFinding pathfinder;

	bool subTargetting = false;
	bool reversing = false;
	int stuckCounter;
	float reverseThresholdSpeed = -15.f;
	int framesReversing = 0;
	int reversingMax = 750;
	int stuckThreshold = 200; //TUNING POINT

	bool recentlyUsedAction = false;
	int actionDelay = 100;
	int actionCounter = 0;

	std::pair<int, int> lastPosition;
	PxVec3 goalPos;
};

