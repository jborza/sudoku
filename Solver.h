#pragma once
#include "Grid.h"
#include "Cell.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

struct HintData{
	bool success;
	Cell* cell; //TODO could be multiple cells
	string name;
	string message;
	int valueToHighlight;
	vector<Cell*> cellsToHighlight;
	map<Cell*, set<int>> eliminationCandidates;
	// possibilities to cross out in multiple other cells too :/

	HintData() : success(false) {

	}
};

class Solver
{
public:
	HintData Hint(Grid *grid);
};

HintData FindNakedPair(Grid* grid);
