#include "Grid.h"
#include <algorithm>
#include <iterator>
#include <set>

using namespace std;

int GetHouseNumber(int row, int col) {
	return (row / 3) * 3 + (col / 3);
}

Grid::Grid() {
	for (int row = 0; row < GRID_HEIGHT; row++)
	{
		for (int col = 0; col < GRID_WIDTH; col++) {
			Cell* cell = new Cell();
			cell->col = col;
			cell->row = row;
			cell->house = GetHouseNumber(row, col);
			cells.push_back(cell);
		}
	}
}

inline int GetCellIndex(int row, int col)
{
	return row * GRID_HEIGHT + col;
}

Cell* Grid::GetCell(int row, int col)
{
	return cells[GetCellIndex(row, col)];
}

int Grid::GetCellValue(int row, int col) {
	auto cell = GetCell(row, col);
	return cell->value;
}

void Grid::SetCellValue(int row, int col, int value)
{
	auto cell = GetCell(row, col);
	cell->SetValue(value);
}

void Grid::AutoNoteUser()
{
	for (Cell* cell : cells)
	{
		this->AutoNoteCell(cell, false);
	}
}

void Grid::AutoNoteSystem()
{
	//original system autonote was bad, because we should apply hints to system hints too, so just copy user hints
	for (Cell* cell : cells)
	{
		this->AutoNoteCell(cell, true);
		if (cell->crossedOutHints.size() > 0)
			for (auto crossedOutHint : cell->crossedOutHints)
				cell->systemHints.erase(crossedOutHint);
	}
}


void Grid::AutoNoteCell(Cell* cell, bool systemHints)
{
	set<int>& hints = systemHints ? cell->systemHints : cell->hints;
	hints.clear();
	if (cell->hasValue())
		return;
	set<int> seen;
	for (int i = 1; i <= 9; i++)
		hints.insert(i);
	//remove visible row cells
	Cell::RemoveCellsWithValue(hints, GetRow(cell->row));
	//remove visible column cells
	Cell::RemoveCellsWithValue(hints, GetColumn(cell->col));
	//remove visible house
	Cell::RemoveCellsWithValue(hints, GetHouse(cell->house));
}

std::vector<Cell*> Grid::GetRow(int row)
{
	vector<Cell*> row_cells;
	copy_if(cells.begin(), cells.end(), std::back_inserter(row_cells), [&](auto c) {return c->row == row; });
	return row_cells;
}

std::vector<Cell*> Grid::GetColumn(int col)
{
	vector<Cell*> row_cells;
	copy_if(cells.begin(), cells.end(), std::back_inserter(row_cells), [&](auto c) {return c->col == col; });
	return row_cells;
}

std::vector<Cell*> Grid::GetHouse(int house)
{
	vector<Cell*> row_cells;
	copy_if(cells.begin(), cells.end(), std::back_inserter(row_cells), [&](auto c) {return c->house == house; });
	return row_cells;
}

std::vector<Cell*> Grid::UnsolvedCells() {
	vector<Cell*> row_cells;
	copy_if(cells.begin(), cells.end(), std::back_inserter(row_cells), [&](auto c) {return c->hasValue() == false; });
	return row_cells;
}

void Grid::RotateRight() {
	vector<Cell*> newCells(cells.size());
	for (int column = 0; column < 9; column++) {
		for (int row = 0; row < 9; row++) {
			//row 8 becomes column 0
			auto cell = cells[GetCellIndex(row, column)];
			cell->row = column;
			cell->col = 8 - row;
			newCells[GetCellIndex(cell->row, cell->col)] = cell;
		}
	}
	cells = newCells;
}