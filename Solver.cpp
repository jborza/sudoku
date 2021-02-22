#include "Solver.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

HintData FindNakedSingle(Grid* grid) {
	HintData data;
	grid->AutoNoteSystem();
	for (Cell* cell : grid->UnsolvedCells())
	{
		if (cell->systemHints.size() == 1) {
			data.name = "Naked single";
			ostringstream oss;
			oss << "Naked single: " << *(cell->systemHints.begin()) << " at (" << cell->col << ", " << cell->row << ")" << endl;
			data.message = oss.str();
			data.cell = cell;
			data.cellsToHighlight.push_back(cell);
			return data;
		}
	}
	return data;
}

HintData FindHiddenSingle(Grid* grid) {
	HintData data;
	grid->AutoNoteSystem();
	for (Cell* cell : grid->cells)
	{
		//only deal with unsolved cells
		if (cell->hasValue)
			continue;

		//system hints are autofilled, find those that are alone in the (row, column, house)
		//remove myself from all directions
		auto house = grid->GetHouse(cell->house);
		Cell::RemoveSelf(house, cell);
		auto row = grid->GetRow(cell->row);
		Cell::RemoveSelf(row, cell);
		auto col = grid->GetColumn(cell->col);
		Cell::RemoveSelf(col, cell);

		if(cell->row == 1 && cell->col == 6)

		for (auto possibility : cell->systemHints)
		{
			int countHouse = std::count_if(house.begin(), house.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });
			int countRow = std::count_if(row.begin(), row.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });
			int countColumn = std::count_if(col.begin(), col.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });
			
			if (countHouse == 0 || countRow == 0  || countColumn == 0) {
				if (countHouse == 0)
					data.message = "It's the only one in its house";
				if (countRow == 0)
					data.message = "It's the only one in its row";
				if (countColumn == 0)
					data.message = "It's the only one in its house";
				data.success = true;
				data.name = "Hidden single";
				data.cell = cell;
				data.cellsToHighlight.push_back(cell);
				return data;
			}
		
		}

	}
	return data;
}

HintData FindNakedPair(Grid* grid) {
	HintData data;
	grid->AutoNoteSystem();
	//iterate through (row, column, house)
	for (int house = 5; house < 9; house++) {
		auto cells = grid->GetHouse(house);
		//find a house with two options
		vector<Cell*> cellsWithTwoOptions;
		std::copy_if(cells.begin(), cells.end(), back_inserter(cellsWithTwoOptions), [](Cell* c) {return c->systemHints.size() == 2; });
		//check every option if there's another one with exactly the same options
		for (auto option : cellsWithTwoOptions)
		{
			for (auto other : cellsWithTwoOptions)
			{
				if (other == option)
					continue;
				if (option->systemHints == other->systemHints)
				{
					//
					data.success = true;
					data.cellsToHighlight.push_back(option);
					data.cellsToHighlight.push_back(other);
					data.name = "Naked pair";
					data.message = "These cells can only be ... Only two in the house";
					//TODO suggest the removals! 
					return data;
				};
			}

		}
	}
	return data;
}

HintData Solver::Hint(Grid* grid)
{
	HintData data;
	data.success = true;
	//iterate through techniques and try to hint from the easiest ones to the hardest ones

	//hint outcomes can be:
	// - crossed out hints
	// - single cell solution (in case of naked or hidden single)

	//first technique - naked single
	data = FindNakedSingle(grid);
	if (data.success)
		return data;

	//second - hidden single
	data = FindHiddenSingle(grid);
	if (data.success)
		return data;

	//naked pair
	data = FindNakedPair(grid);
	if (data.success)
		return data;

	//locked candidate


	data.success = false;
	return data;
}