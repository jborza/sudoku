#include "Cell.h"
#include <sstream>
#include <algorithm>
#include "set_tools.h"

using namespace std;

void Cell::See(vector<Cell*> cells)
{
	Cell::RemoveCellsWithValue(hints, cells);
}

template<typename T>
void Cell::RemoveCellsWithValue(std::set<T>& someSet, std::vector<Cell*> cells){
	for(auto item : cells)
		if(item->hasValue)
			someSet.erase(item->value);
}

void Cell::RemoveSelf(std::vector<Cell*>& cells, Cell* cell) {
	cells.erase(std::remove(cells.begin(), cells.end(), cell));
}

vector<Cell*> Cell::Except(vector<Cell*> cells, initializer_list<Cell*> exclusions)
{
	vector<Cell*> dest;
	copy_if(cells.begin(), cells.end(), back_inserter(dest), [&](auto c) {return !contains(exclusions, c); });
	return dest;
}

std::string Cell::CoordsToString(){
	ostringstream oss;
	oss << "(" << col << ", " << row << ")";
	return oss.str();
}

bool Cell::SharesRowWith(Cell* b)
{
	return this->row == b->row;
}

bool Cell::SharesColumnWith(Cell* b)
{
	return this->col == b->col;
}
