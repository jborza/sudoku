#include "Solver.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;



HintData Solver::Hint(Grid *grid)
{
	HintData data;
	data.success = true;
	//iterate through techniques and try to hint from the easiest ones to the hardest ones

	//hint outcomes can be:
	// - crossed out hints
	// - single cell solution (in case of naked or hidden single)

	//first technique - naked single
	grid->AutoNoteSystem();
	for (Cell *cell : grid->UnsolvedCells())
	{
		if(cell->systemHints.size() == 1){
			data.name = "Naked single";
			ostringstream oss;
			oss << "Naked single: " << *(cell->systemHints.begin()) << " at (" << cell->col << ", " << cell->row << ")" << endl;
			data.message = oss.str();
			data.cell = cell;
			data.cellsToHighlight.push_back(cell);
			return data;
		}
	}

	//second - hidden single
	grid->AutoNoteSystem();
	for (Cell *cell : grid->cells)
	{
		//only deal with unsolved cells
		if (cell->hasValue)
			continue;
		
		//system hints are autofilled, find those that are alone in the (row, column, house)
		//check in the house
		auto house = grid->GetHouse(cell->house);
		//remove myself
		

	}

	data.success = false;
	return data;
}