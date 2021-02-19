#include "Cell.h"

void Cell::See(std::vector<Cell*> cells)
{
	for (auto cell : cells)
		if (cell->hasValue)
			hints.erase(cell->value);
}
