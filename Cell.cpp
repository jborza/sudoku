#include "Cell.h"
#include <sstream>
#include <algorithm>

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

std::string Cell::CoordsToString(){
	ostringstream oss;
	oss << "(" << col << ", " << row << ")";
	return oss.str();
}
