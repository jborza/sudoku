#pragma once
#include "Grid.h"
#include "Cell.h"
#include <string>
#include <vector>

using namespace std;

struct HintData{
	bool success;
	Cell* cell; //TODO could be multiple cells
	string name;
	string message;
	int valueToHighlight;
	std::vector<Cell*> cellsToHighlight;
	// possibilities to cross out in multiple other cells too :/
};

class Solver
{
public:
	HintData Hint(Grid *grid);
};

