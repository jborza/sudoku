// sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Game.h"
#include "sudoku.h"

int main()
{
	Game game;
	std::cout << "Loading puzzle...!\n";
	game.Load("easy.txt");
	game.Print();
	game.grid.AutoNote();

	for (int col = 0; col < 8; col++)
		game.PrintNote(col, 0);
	game.PrintNote(0, 1);
	game.PrintNote(8, 0);
	game.PrintNote(8, 8);
}

