#include "PathFinding.h"

#include <cmath>
#include <set>
#include <fmt/core.h>

// https://www.geeksforgeeks.org/a-search-algorithm/ <---- slightly modified this implementation

bool PathFinding::isValid(int row, int col)
{
	return (row >= 0) && (row < ROW) && (col >= 0)
		&& (col < COL);
}

bool PathFinding::isUnBlocked(int grid[][COL], int row, int col)
{
	if (grid[row][col] >= 1)
		return true;
	return false;
}

bool PathFinding::isDestination(int row, int col, Pair dest)
{
	if (row == dest.first && col == dest.second)
		return (true);
	return (false);
}

double PathFinding::calculateHValue(int row, int col, Pair dest) const
{
	return std::sqrt(std::pow(row - dest.first, 2) + std::pow(col - dest.second, 2)); //euclidean distance
}


std::stack<Pair> PathFinding::tracePath(Cell cellDetails[][COL], Pair dest)
{
	int row = dest.first;
	int col = dest.second;

	std::stack<Pair> Path;

	while (!(cellDetails[row][col].parent_i == row
		&& cellDetails[row][col].parent_j == col))
	{
		Path.push(std::make_pair(row, col));
		int temp_row = cellDetails[row][col].parent_i;
		int temp_col = cellDetails[row][col].parent_j;
		row = temp_row;
		col = temp_col;
	}
	Path.push(std::make_pair(row, col));

	return Path;
}

std::stack<Pair> PathFinding::aStarSearch(int grid[][COL], Pair src, Pair dest) const
{
	// If the source is out of range
	if (isValid(src.first, src.second) == false)
	{
		std::stack<Pair> dummy;
		dummy.push(std::make_pair(-1, -1));
		return dummy;
	}

	// If the destination is out of range
	if (isValid(dest.first, dest.second) == false)
	{
		std::stack<Pair> dummy;
		dummy.push(std::make_pair(-1, -1));
		return dummy;
	}

	// Either the source or the destination is blocked
	if (isUnBlocked(grid, src.first, src.second) == false)
	{
		std::stack<Pair> dummy;
		dummy.push(std::make_pair(-1, -1));
		return dummy;
	}

	if (isUnBlocked(grid, dest.first, dest.second) == false)
	{
		std::stack<Pair> dummy;
		dummy.push(std::make_pair(-2, -2));
		return dummy;
	}


	// If the destination cell is the same as source cell
	if (isDestination(src.first, src.second, dest)
		== true)
	{
		std::stack<Pair> dummy;
		dummy.push(std::make_pair(0, 0));
		return dummy;
	}

	bool closedList[ROW][COL];
	memset(closedList, false, sizeof(closedList));

	Cell cellDetails[ROW][COL];

	int i, j;

	for (i = 0; i < ROW; i++)
	{
		for (j = 0; j < COL; j++)
		{
			cellDetails[i][j].f = FLT_MAX;
			cellDetails[i][j].g = FLT_MAX;
			cellDetails[i][j].h = FLT_MAX;
			cellDetails[i][j].parent_i = -1;
			cellDetails[i][j].parent_j = -1;
		}
	}

	i = src.first, j = src.second;
	cellDetails[i][j].f = 0.0;
	cellDetails[i][j].g = 0.0;
	cellDetails[i][j].h = 0.0;
	cellDetails[i][j].parent_i = i;
	cellDetails[i][j].parent_j = j;

	std::set<pPair> openList;

	openList.insert(std::make_pair(0.0, std::make_pair(i, j)));

	bool foundDest = false;

	while (!openList.empty())
	{
		pPair p = *openList.begin();

		openList.erase(openList.begin());

		i = p.second.first;
		j = p.second.second;
		closedList[i][j] = true;


		// To store the 'g', 'h' and 'f' of the 8 successors
		double gNew, hNew, fNew;

		//----------- 1st Successor (North) ------------
		if (isValid(i - 1, j) == true)
		{
			if (isDestination(i - 1, j, dest) == true)
			{
				cellDetails[i - 1][j].parent_i = i;
				cellDetails[i - 1][j].parent_j = j;
				foundDest = true;
				return tracePath(cellDetails, dest);
			}
			if (closedList[i - 1][j] == false
				&& isUnBlocked(grid, i - 1, j)
				== true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i - 1, j, dest);
				if (grid[i - 1][j] > 1) hNew = hNew + 10;
				fNew = gNew + hNew;

				if (cellDetails[i - 1][j].f == FLT_MAX
					|| cellDetails[i - 1][j].f > fNew)
				{
					openList.insert(std::make_pair(
						fNew, std::make_pair(i - 1, j)));

					cellDetails[i - 1][j].f = fNew;
					cellDetails[i - 1][j].g = gNew;
					cellDetails[i - 1][j].h = hNew;
					cellDetails[i - 1][j].parent_i = i;
					cellDetails[i - 1][j].parent_j = j;
				}
			}
		}

		//----------- 2nd Successor (South) ------------
		if (isValid(i + 1, j) == true)
		{
			if (isDestination(i + 1, j, dest) == true)
			{
				cellDetails[i + 1][j].parent_i = i;
				cellDetails[i + 1][j].parent_j = j;
				foundDest = true;
				return tracePath(cellDetails, dest);
			}
			if (closedList[i + 1][j] == false
				&& isUnBlocked(grid, i + 1, j)
				== true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i + 1, j, dest);
				if (grid[i + 1][j] > 1) hNew = hNew + 10;
				fNew = gNew + hNew;


				if (cellDetails[i + 1][j].f == FLT_MAX
					|| cellDetails[i + 1][j].f > fNew)
				{
					openList.insert(std::make_pair(
						fNew, std::make_pair(i + 1, j)));

					cellDetails[i + 1][j].f = fNew;
					cellDetails[i + 1][j].g = gNew;
					cellDetails[i + 1][j].h = hNew;
					cellDetails[i + 1][j].parent_i = i;
					cellDetails[i + 1][j].parent_j = j;
				}
			}
		}


		//----------- 3rd Successor (East) ------------
		if (isValid(i, j + 1) == true)
		{
			if (isDestination(i, j + 1, dest) == true)
			{
				cellDetails[i][j + 1].parent_i = i;
				cellDetails[i][j + 1].parent_j = j;
				foundDest = true;
				return tracePath(cellDetails, dest);
			}
			if (closedList[i][j + 1] == false
				&& isUnBlocked(grid, i, j + 1)
				== true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i, j + 1, dest);
				if (grid[i][j + 1] > 1) hNew = hNew + 10;
				fNew = gNew + hNew;

				if (cellDetails[i][j + 1].f == FLT_MAX
					|| cellDetails[i][j + 1].f > fNew)
				{
					openList.insert(std::make_pair(
						fNew, std::make_pair(i, j + 1)));

					cellDetails[i][j + 1].f = fNew;
					cellDetails[i][j + 1].g = gNew;
					cellDetails[i][j + 1].h = hNew;
					cellDetails[i][j + 1].parent_i = i;
					cellDetails[i][j + 1].parent_j = j;
				}
			}
		}

		//----------- 4th Successor (West) ------------
		if (isValid(i, j - 1) == true)
		{
			if (isDestination(i, j - 1, dest) == true)
			{
				cellDetails[i][j - 1].parent_i = i;
				cellDetails[i][j - 1].parent_j = j;
				foundDest = true;
				return tracePath(cellDetails, dest);
			}
			if (closedList[i][j - 1] == false
				&& isUnBlocked(grid, i, j - 1)
				== true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i, j - 1, dest);
				if (grid[i - 1][j] - 1) hNew = hNew + 10;
				fNew = gNew + hNew;


				if (cellDetails[i][j - 1].f == FLT_MAX
					|| cellDetails[i][j - 1].f > fNew)
				{
					openList.insert(std::make_pair(
						fNew, std::make_pair(i, j - 1)));

					cellDetails[i][j - 1].f = fNew;
					cellDetails[i][j - 1].g = gNew;
					cellDetails[i][j - 1].h = hNew;
					cellDetails[i][j - 1].parent_i = i;
					cellDetails[i][j - 1].parent_j = j;
				}
			}
		}

		//----------- 5th Successor (North-East)
		if (isValid(i - 1, j + 1) == true)
		{
			if (isDestination(i - 1, j + 1, dest) == true)
			{
				cellDetails[i - 1][j + 1].parent_i = i;
				cellDetails[i - 1][j + 1].parent_j = j;
				foundDest = true;
				return tracePath(cellDetails, dest);
			}
			if (closedList[i - 1][j + 1] == false
				&& isUnBlocked(grid, i - 1, j + 1)
				== true)
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i - 1, j + 1, dest);
				if (grid[i - 1][j + 1] > 1) hNew = hNew + 10;
				fNew = gNew + hNew;

				if (cellDetails[i - 1][j + 1].f == FLT_MAX
					|| cellDetails[i - 1][j + 1].f > fNew)
				{
					openList.insert(std::make_pair(
						fNew, std::make_pair(i - 1, j + 1)));

					cellDetails[i - 1][j + 1].f = fNew;
					cellDetails[i - 1][j + 1].g = gNew;
					cellDetails[i - 1][j + 1].h = hNew;
					cellDetails[i - 1][j + 1].parent_i = i;
					cellDetails[i - 1][j + 1].parent_j = j;
				}
			}
		}

		//----------- 6th Successor (North-West)
		if (isValid(i - 1, j - 1) == true)
		{
			if (isDestination(i - 1, j - 1, dest) == true)
			{
				cellDetails[i - 1][j - 1].parent_i = i;
				cellDetails[i - 1][j - 1].parent_j = j;
				foundDest = true;
				return tracePath(cellDetails, dest);
			}
			if (closedList[i - 1][j - 1] == false
				&& isUnBlocked(grid, i - 1, j - 1)
				== true)
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i - 1, j - 1, dest);
				if (grid[i - 1][j - 1] > 1) hNew = hNew + 10;
				fNew = gNew + hNew;

				if (cellDetails[i - 1][j - 1].f == FLT_MAX
					|| cellDetails[i - 1][j - 1].f > fNew)
				{
					openList.insert(std::make_pair(
						fNew, std::make_pair(i - 1, j - 1)));

					cellDetails[i - 1][j - 1].f = fNew;
					cellDetails[i - 1][j - 1].g = gNew;
					cellDetails[i - 1][j - 1].h = hNew;
					cellDetails[i - 1][j - 1].parent_i = i;
					cellDetails[i - 1][j - 1].parent_j = j;
				}
			}
		}

		//----------- 7th Successor (South-East)
		if (isValid(i + 1, j + 1) == true)
		{
			if (isDestination(i + 1, j + 1, dest) == true)
			{
				// Set the Parent of the destination cell
				cellDetails[i + 1][j + 1].parent_i = i;
				cellDetails[i + 1][j + 1].parent_j = j;
				foundDest = true;
				return tracePath(cellDetails, dest);
			}
			if (closedList[i + 1][j + 1] == false
				&& isUnBlocked(grid, i + 1, j + 1)
				== true)
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i + 1, j + 1, dest);
				if (grid[i + 1][j + 1] > 1) hNew = hNew + 10;
				fNew = gNew + hNew;

				if (cellDetails[i + 1][j + 1].f == FLT_MAX
					|| cellDetails[i + 1][j + 1].f > fNew)
				{
					openList.insert(std::make_pair(
						fNew, std::make_pair(i + 1, j + 1)));

					cellDetails[i + 1][j + 1].f = fNew;
					cellDetails[i + 1][j + 1].g = gNew;
					cellDetails[i + 1][j + 1].h = hNew;
					cellDetails[i + 1][j + 1].parent_i = i;
					cellDetails[i + 1][j + 1].parent_j = j;
				}
			}
		}

		//----------- 8th Successor (South-West)
		if (isValid(i + 1, j - 1) == true)
		{
			if (isDestination(i + 1, j - 1, dest) == true)
			{
				cellDetails[i + 1][j - 1].parent_i = i;
				cellDetails[i + 1][j - 1].parent_j = j;
				foundDest = true;
				return tracePath(cellDetails, dest);
			}
			if (closedList[i + 1][j - 1] == false
				&& isUnBlocked(grid, i + 1, j - 1)
				== true)
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i + 1, j - 1, dest);
				if (grid[i + 1][j - 1] > 1) hNew = hNew + 10;
				fNew = gNew + hNew;


				if (cellDetails[i + 1][j - 1].f == FLT_MAX
					|| cellDetails[i + 1][j - 1].f > fNew)
				{
					openList.insert(std::make_pair(
						fNew, std::make_pair(i + 1, j - 1)));

					// Update the details of this cell
					cellDetails[i + 1][j - 1].f = fNew;
					cellDetails[i + 1][j - 1].g = gNew;
					cellDetails[i + 1][j - 1].h = hNew;
					cellDetails[i + 1][j - 1].parent_i = i;
					cellDetails[i + 1][j - 1].parent_j = j;
				}
			}
		}
	}

	if (foundDest == false)
		fmt::print("Failed to find the Destination Cell\n");

	std::stack<Pair> dummy;
	return dummy;
}

