#include "Solver.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "set_tools.h"
#include "Cells.h"
#include <ranges>

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
		if (cell->hasValue())
			continue;

		//system hints are autofilled, find those that are alone in the (row, column, house)
		//remove myself from all directions
		auto house = Cell::Except(grid->GetHouse(cell->house), { cell });
		auto row = Cell::Except(grid->GetRow(cell->row), { cell });
		auto col = Cell::Except(grid->GetColumn(cell->col), { cell });

		for (auto possibility : cell->systemHints)
		{
			int countHouse = std::count_if(house.begin(), house.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });
			int countRow = std::count_if(row.begin(), row.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });
			int countColumn = std::count_if(col.begin(), col.end(), [&](Cell* c) {return c->systemHints.find(possibility) != c->systemHints.end(); });

			if (countHouse == 0 || countRow == 0 || countColumn == 0) {

				data.success = true;
				data.name = "Hidden single";
				data.valueToHighlight = possibility;
				ostringstream oss;
				oss << "The cell should be " << possibility << ", as it's the only one in its ";

				//todo reasonable separator
				vector<string> foundLocations;
				if (countHouse == 0)
					foundLocations.push_back("house");
				if (countRow == 0)
					foundLocations.push_back("row");
				if (countColumn == 0)
					foundLocations.push_back("column");
				oss << comma_separated(foundLocations);
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
			auto& s = eliminationCandidates[cell];
			s.insert(candidate);
		}
	}
}

vector<Cell*> cellsWithNOptions(int n, vector<Cell*> cells) {
	vector<Cell*> cellsWithOptions;
	std::copy_if(cells.begin(), cells.end(), back_inserter(cellsWithOptions), [](Cell* c) {return c->systemHints.size() == 2; });
	return cellsWithOptions;
}

HintData FindNakedPair(Grid* grid) {
	HintData data;
	grid->AutoNoteSystem();
	//iterate through house only for naked pairs
	for (int house = 0; house < 9; house++) {
		auto cells = grid->GetHouse(house);
		//find a house with two options
		vector<Cell*> cellsWithTwoOptions = cellsWithNOptions(2, cells);
		//std::copy_if(cells.begin(), cells.end(), back_inserter(cellsWithTwoOptions), [](Cell* c) {return c->systemHints.size() == 2; });
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
					//TODO don't say success if there are no elimination candidates!!!
					data.success = true;
					data.cellsToHighlight.push_back(option);
					data.cellsToHighlight.push_back(other);
					data.eliminationCandidates = eliminationCandidates;
					data.name = "Naked pair";
					ostringstream oss;
					oss << "These cells can only be " << first << ", " << second;
					oss << " as they are the only two in the house.";
					data.message = oss.str();
					return data;
				};
			}

		}
	}
	return data;
}

HintData FindPointingTuple(Grid* grid) {
	HintData data;
	grid->AutoNoteSystem();
	//iterate through house only for the only cells that can form a row or column
	for (int house = 0; house < 9; house++) {
		auto cells = grid->GetHouse(house);
		// build a map of: option -> cells
		//TODO extract
		map<int, vector<Cell*>> cellsByCandidates;
		for (int candidate = 1; candidate <= 9; candidate++)
		{
			for (Cell* cell : cells)
				if (contains(cell->systemHints, candidate))
					cellsByCandidates[candidate].push_back(cell);
		}

		string foundFormation;
		//TODO refactor
		for (auto& cc : cellsByCandidates) {
			//groups with size 1 should not happen as they should get caught by singles
			if (cc.second.size() < 2)
				continue;
			auto candidate = cc.first;
			auto first = *(cc.second.begin());
			if (Cell::AllShareRow(cc.second)) {
				// extend this row and eliminate
				auto otherCells = Cell::Except(grid->GetRow(first->row), cc.second);
				foundFormation = "row";
				AddEliminationCandidates(data.eliminationCandidates, otherCells, candidate);
			}
			if (Cell::AllShareColumn(cc.second)) {
				// extend this column and eliminate
				auto otherCells = Cell::Except(grid->GetColumn(first->col), cc.second);
				foundFormation = "column";
				AddEliminationCandidates(data.eliminationCandidates, otherCells, candidate);
			}
			if (data.eliminationCandidates.size() > 0) {
				data.cellsToHighlight = cc.second;
				data.valueToHighlight = candidate;
				data.name = cc.second.size() == 3 ? "Pointing triple" : "Pointing double";
				data.message = "They are the only ones in their house, so we can eliminate other candidates from their " + foundFormation;
				data.success = true;
				return data;
			}
		}
	}
	return data;
}

HintData FindLockedCandidate(Grid* grid) {
	HintData data;
	grid->AutoNoteSystem();
	//rows!
	for (int candidate = 1; candidate <= 9; candidate++) {
		for (int row = 0; row < 9; row++)
		{
			auto rowCells = grid->GetRow(row);
			//check if candidate options for this row share a house
			auto rowOptions = GetCellsWithSystemHint(rowCells, candidate);
			if (rowOptions.size() < 2)
				continue;
			//if yes, we can eliminate other from the house
			auto hasHint = [&](Cell* c) {return contains(c->systemHints, candidate); };
			int possibleHouse = rowOptions[0]->house;
			bool shareSameHouse = std::all_of(rowOptions.begin(), rowOptions.end(), [&](auto c) {return c->house == possibleHouse; });
			if (!shareSameHouse)
				continue;
			//check for elimination candidates
			auto houseCells = Cell::Except(grid->GetHouse(possibleHouse), rowOptions);
			auto houseCellsWithCandidate = GetCellsWithSystemHint(houseCells, candidate);
			if (houseCellsWithCandidate.size() == 0)
				continue;
			AddEliminationCandidates(data.eliminationCandidates, houseCellsWithCandidate, candidate);
			data.cellsToHighlight = rowOptions;
			data.name = "Locked candidate";
			data.message = "TODO Locked candidate description";
			data.success = true;
			return data;
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

	//pointing double/triple
	data = FindPointingTuple(grid);
	if (data.success)
		return data;

	//locked candidate
	data = FindLockedCandidate(grid);
	if (data.success)
		return data;

	data.success = false;
	return data;
}

bool Solver::ApplyHint(Grid* grid, HintData hint) {
	if (!hint.success)
		return false;

	cout << "Hint:" << hint.name << endl;
	cout << hint.message << endl;
	cout << "Affects cells at: ";
	for (auto cell : hint.cellsToHighlight)
		cout << cell->CoordsToString() << " ";
	cout << endl;
	//eliminate the elimination candidates
	for (auto candidate : hint.eliminationCandidates)
	{
		auto cell = candidate.first;
		cout << "Elimination candidate at " << cell->CoordsToString() << ": ";
		auto options = candidate.second;
		cout << comma_separated(options) << endl;
		cell->crossedOutHints.insert(options.begin(), options.end());

	}

	return true;
}