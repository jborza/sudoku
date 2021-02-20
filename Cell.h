#pragma once
#include <set>
#include <vector>
class Cell
{
public:
	int value;
	std::set<int> hints;
	std::set<int> accentHints;
	std::set<int> crossedOutHints;
	bool fixedFromStart;
	bool hasValue;
	int row;
	int col;
	int house;
	void See(std::vector<Cell*> cells);
};

