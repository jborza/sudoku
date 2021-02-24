#include "Solver.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "set_tools.h"

using namespace std;

HintData FindNakedSingle(Grid* grid) {
	HintData data;
	grid->AutoNoteSystem();
	for (Cell* cell : grid->UnsolvedCells())
	{
		if (cell->systemHints.size() == 1) {
			data.name = "Naked single";
			ostringstream oss;
			int value = *(cell->systemHints.begin());
			oss << "Naked single: " << value << " at (" << cell->col << ", " << cell->row << ")" << endl;
			data.message = oss.str();
			data.cell = cell;
			data.valueToHighlight = value;
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

		for (auto possibility : cell->systemHints)
		{
			int countHouse = std::count_if(house.begin(), house.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });
			int countRow = std::count_if(row.begin(), row.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });
			int countColumn = std::count_if(col.begin(), col.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });
			
			if (countHouse == 0 || countRow == 0  || countColumn == 0) {
				
				data.success = true;
				data.name = "Hidden single";
				data.valueToHighlight = possibility;
				ostringstream oss;
				oss << "The cell should be " << possibility << ", as it's the only one in its ";

				if (countHouse == 0)
					oss << "house";
				else if (countRow == 0)
					oss << "row";
				else if (countColumn == 0)
					oss << "column";
				data.message = oss.str();
				data.cell = cell;
				data.cellsToHighlight.push_back(cell);
				return data;
			}
		
		}

	}
	return data;
}

//TODO definitely can reuse for naked triple
void AddEliminationCandidates(map<Cell*, set<int>>& eliminationCandidates, vector<Cell*> cells, int candidate) {
	for (auto cell : cells) {
		if (contains(cell->systemHints, candidate)) {
			//cout << "Found removal candidate " << candidate << " in " << cell->CoordsToString() << endl;
			auto& s = eliminationCandidates[cell];
			s.insert(candidate);
		}
	}
}

HintData FindNakedPair(Grid* grid) {
	HintData data;
	grid->AutoNoteSystem();
	//iterate through house only for naked pairs
	for (int house = 0; house < 9; house++) {
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
					
					//if so, can we eliminate anything from said row and column?
					int first = *std::next(option->systemHints.begin(), 0);
					int second = *std::next(option->systemHints.begin(), 1);

					map<Cell*, set<int>> eliminationCandidates;
					
					//can we eliminate anything from this house? (test with test-nakedpair-house.txt)
					auto houseCells = Cell::Except(cells, { option, other });
					AddEliminationCandidates(eliminationCandidates, houseCells, first);
					AddEliminationCandidates(eliminationCandidates, houseCells, second);
					//also, are the items arranged in a a "row", "column"? (test with test-nakedpair-col.txt / test-nakedpair-row.txt)
					if (option->SharesColumnWith(other)) {
						auto columnCells = Cell::Except(grid->GetColumn(option->col), { option, other });
						AddEliminationCandidates(eliminationCandidates, columnCells, first);
						AddEliminationCandidates(eliminationCandidates, columnCells, second);
					}
					if (option->SharesRowWith(other)) {
						auto rowCells = Cell::Except(grid->GetRow(option->row), { option, other });
						AddEliminationCandidates(eliminationCandidates, rowCells, first);
						AddEliminationCandidates(eliminationCandidates, rowCells, second);
					}
					//
					data.success = true;
					data.cellsToHighlight.push_back(option);
					data.cellsToHighlight.push_back(other);
					data.eliminationCandidates = eliminationCandidates;
					data.name = "Naked pair";
					ostringstream oss;
					oss << "These cells can only be " << first << ", " << second;
					oss << " as they are the only two in the house.";
					data.message = oss.str();
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