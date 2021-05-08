#pragma once

#include <utility>
#include <stack>

constexpr int ROW = 26;
constexpr int COL = 26;

// Creating a shortcut for int, int pair type
typedef std::pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef std::pair<double, std::pair<int, int> > pPair;

struct Cell {
	int parent_i, parent_j;
	double f, g, h;
};

class PathFinding
{
public:
	std::stack<Pair> aStarSearch(int grid[][COL], Pair, Pair) const;

private:
	static bool isValid(int, int);
	static bool isUnBlocked(int grid[][COL], int, int);
	static bool isDestination(int, int, Pair);
	double calculateHValue(int, int, Pair) const;
	static std::stack<Pair> tracePath(Cell [][COL], Pair);
};

