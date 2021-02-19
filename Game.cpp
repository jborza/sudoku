#include "Game.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
void Game::Print() {
	//print header
	cout << " /--------------------\\" << endl;
	//first 3 rows
	for (int row = 0; row < 3; row++) 
		PrintRow(row);
	cout << " ----------------------" << endl;
	//next 3 rows
	for (int row = 3; row < 6; row++)
		PrintRow(row);
	cout << " ----------------------" << endl;
	//end
	for (int row = 6; row < 9; row++)
		PrintRow(row);
	cout << " \\--------------------/" << endl;
}

void Game::PrintNote(int col, int row)
{
	cout << "Notes for (" << col << ", " << row << "):";
	auto cell = grid.GetCell(row, col);
	for (auto note : cell->hints)
		cout << note << " ";
	cout << endl;
}

void print_cell(Cell* cell) {
	if (cell->hasValue)
		cout << cell->value << " ";
	else
		cout << "  ";
}

void Game::PrintRow(int row)
{
	cout << "||";
	for (int col = 0; col < 3; col++)
		print_cell(grid.GetCell(row, col));
	cout << "|";
	for (int col = 3; col < 6; col++)
		print_cell(grid.GetCell(row, col));
	cout << "|";
	for (int col = 6; col < 9; col++)
		print_cell(grid.GetCell(row, col));
	cout << "||" << endl;
}

void Game::Load(const char* name) {
	ifstream file(name);
	std::string str;
	int row = 0;
	while (std::getline(file, str))
	{
		//row by row
		for (int col = 0; col < GRID_WIDTH; col++) {
			auto digit = str[col];
			int value = digit - '0';
			if (digit != ' ')
				grid.SetCellValue(row, col, value);
		}
		row++;
	}
	grid.GetCell(0, 0)->hints.insert(1);
	grid.GetCell(0, 0)->hints.insert(6);
	grid.GetCell(0, 0)->hints.insert(9);
}