#include "Grid.h"
Grid::Grid() {
	for (int y = 0; y < GRID_HEIGHT; y++)
	{
		for (int x = 0; x < GRID_WIDTH; x++) {
			Cell* cell = new Cell();
			cells.push_back(cell);
		}
	}
}

Cell* Grid::GetCell(int row, int col)
{
	return cells[row * GRID_HEIGHT + col];
}

int Grid::GetCellValue(int row, int col) {
	auto cell = GetCell(row, col);
	return cell->value;
}

void Grid::SetCellValue(int row, int col, int value)
{
	auto cell = GetCell(row, col);
	cell->hasValue = true;
	cell->value = value;
}
