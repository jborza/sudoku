#include "Solver.h"
#include <algorithm>
#include <iostream>

using namespace std;

template<typename T>
void RemoveCellsWithValue(std::set<T>& someSet, std::vector<Cell*> cells){
	for(auto item : cells)
		if(item->hasValue)
			someSet.erase(item->value);
}

void Solver::Hint(Grid *grid)
{
	//iterate through techniques and try to hint from the easiest ones to the hardest ones

	//hint outcomes can be:
	// - crossed out hints
	// - single cell solution (in case of naked or hidden single)

	//first technique - naked single

	for (Cell *cell : grid->cells)
	{
		//only deal with unsolved cells
		if (cell->hasValue)
			continue;
		//make up a vector of possibilities
		std::set<int> possibilities;
		for (int i = 1; i <= 9; i++)
			possibilities.insert(i);
		//remove visible row cells
		RemoveCellsWithValue(possibilities,  grid->GetRow(cell->row));
		RemoveCellsWithValue(possibilities,  grid->GetColumn(cell->col));
		RemoveCellsWithValue(possibilities,  grid->GetHouse(cell->house));
		if(possibilities.size() == 1){
			std::cout << "HINT: Found a naked single: " << *(possibilities.begin()) << "at (" << cell->col << ", " << cell->row << ")" << endl;
			return;
		}

	}
}