#pragma once
#include <utility>
#include <stack>
#include <set>
#include <math.h>
#include <fmt/format.h>
#include "GlobalState.h"

#define ROW 20
#define COL 20

// Creating a shortcut for int, int pair type
typedef std::pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef std::pair<double, std::pair<int, int> > pPair;

struct Cell {
	int parent_i, parent_j;
	double f, g, h;
};

class Pathfinding
{
public:
	Pathfinding();
	std::stack<Pair> ehStarSearch(int grid[][COL], Pair src, Pair dest);
private:
	bool isValid(int row, int col);
	bool isUnBlocked(int grid[][COL], int row, int col);
	bool isDestination(int row, int col, Pair dest);
	double calculateHValue(int row, int col, Pair dest);
	std::stack<Pair> tracePath(Cell cellDetails[][COL], Pair dest);
};



