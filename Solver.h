#pragma once
#include "Grid.h"
#include "Cell.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

struct HintData{
	bool success;
	Cell* cellToSolve; //TODO could be multiple cells
	string name;
	string message;
	int valueToHighlight;
	vector<Cell*> cellsToHighlight;
	map<Cell*, set<int>> eliminationCandidates;
	// possibilities to cross out in multiple other cells too :/

	HintData() : success(false), cellToSolve(nullptr) {

	}
};

class Solver
{
public:
	HintData Hint(Grid *grid);
	bool ApplyHint(Grid* grid, HintData data);
};

HintData FindLockedCandidate(Grid* grid);
