#pragma once
#include <vector>
#include "Cell.h"

#define GRID_WIDTH 9
#define GRID_HEIGHT 9

class Grid
{
	//a grid has 81 cells, arranged into: Houses, Rows and Columns
public:
	std::vector<Cell*> cells;
	Grid();
	Cell* GetCell(int row, int col);
	int GetCellValue(int row, int col);
	void SetCellValue(int row, int col, int value);
	void AutoNoteUser();
	void AutoNoteSystem();
	void AutoNoteCell(Cell* cell, bool system);
	std::vector<Cell*>  GetRow(int row);
	std::vector<Cell*> GetColumn(int row);
	std::vector<Cell*> GetHouse(int house);
	std::vector<Cell*> UnsolvedCells();
};

