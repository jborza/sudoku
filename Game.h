#pragma once
#include "Grid.h"
class Game
{
public:
	Grid grid;
	void Print();

	void PrintNote(int col, int row);

	
	void PrintRow(int row);
	
	void Load(const char* name);
};

