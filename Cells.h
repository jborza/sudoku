#pragma once
#include <vector>
#include "Cell.h"

using namespace std;

vector<Cell*> GetCellsWithSystemHint(vector<Cell*>& cells, int systemHint) {
	vector<Cell*> dest;
	copy_if(cells.begin(), cells.end(), back_inserter(dest), [&](Cell* c) {return contains(c->systemHints, systemHint); });
	return dest;
}